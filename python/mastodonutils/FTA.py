#!/usr/bin/env python
# Written by Saran Srikanth Bodda, NC State University. Email: ssbodda@ncsu.edu
"""
    Provides FaultTree class for creating fault trees and minimal cut sets given logic of fault tree.

    Quantification class for calculating minimal cut set probabilities and system level probability
        given basic event probabilities.
"""
import os

class Event(object):
    """
    A basic node object for creating fault tree (see FaultTree)

    Inputs:
        name[str]: The name of the Event
        operator[Event.OR|Event.AND|None]: (optional) The node operator, use None for basic
                                                   event nodes.
        prob[list]: [Mean/Median,sd] for Basic events,
                        use None for Intermediate events.
    """
    # Define the node operators
    OR = 0
    AND = 1

    def __init__(self, name, operator=None, prob=None):
        self.__name = name
        self.__children = []
        self.__operator = None
        self.operator = operator
        self.__prob = []
        self.__dist = None

    @property
    def name(self):
        """Return the name attribute"""
        return self.__name

    @property
    def operator(self):
        """Return the operator attribute"""
        return self.__operator

    @operator.setter
    def operator(self, value):
        """Set the operator attribute"""
        if value not in [Event.AND, Event.OR, None]:
            raise TypeError("The operator must be one of the following: Event.AND, "
                            "Event.OR, or None")
        if self.__children and value is None:
            raise AttributeError("The operator can not be set to None (i.e., a basic node) because "
                                 "the Event has children.")
        self.__operator = value

    @property
    def children(self):
        """Return the children attribute"""
        return self.__children

    @children.setter
    def children(self, values):
        """Set the children attribute"""
        if not isinstance(values, list):
            raise TypeError("The supplied items must be a list.")
        if not all(isinstance(v, Event) for v in values):
            raise TypeError("Supplied values must be Event objects.")
        if self.__operator is None:
            raise AttributeError("The node is a basic node (i.e., the operator attribute is not "
                                 "set), therefore it can not have children.")
        self.__children = values

    @property
    def dist(self):
        """Return the distribution attribute"""
        return self.__dist

    @dist.setter
    def dist(self,dist):
        """Set the distribution attribute"""
        distribution = dist[0]
        antype = dist[1]
        if self.__operator is not None:
            raise AttributeError("'%s' is not a basic node (i.e., the operator attribute is "
                                 "set), therefore it can not have distribution." % (self.__name))
        if antype == 'Fragility':
            if distribution not in ['LNORM']:
                raise TypeError("The dist. of '%s' for fragility must be one of the following: LNORM" %(self.__name))
        else:
            if distribution not in ['PE', 'NORM']:
                raise TypeError("The dist. of '%s' for risk must be one of the following: PE, NORM" %(self.__name))
        self.__dist = distribution

    @property
    def prob(self):
        """Return the probability attribute"""
        return self.__prob

    @prob.setter
    def prob(self,values):
        """Set the probability attribute"""
        self.__prob=[]
        prob = values[0]
        antype = values[1]
        im = values[2]
        uc = values[3]
        nsamp = values[4]
        seed = values[5]
        if not isinstance(prob, list):
            raise TypeError("The supplied items of '%s' must be a list." %(self.__name))

        import numpy,scipy.stats
        numpy.random.seed(seed)
        if antype == "Fragility":
            if self.__dist == 'LNORM':
                if len(prob) != 2:
                    raise IndexError("The distribution of '%s' requires two inputs: Mean/Median and Standard_Deviaton" %(self.__name))
                for i in range(0,len(im)):
                    self.__prob.append(scipy.stats.norm.cdf(numpy.log(im[i]/prob[0])/prob[1]))
        else:
            if self.__dist == 'PE':
                if len(prob) != 1:
                    raise IndexError("Point Estimate of '%s' requires only one input: Mean/Median probability" %(self.__name))
                if (prob[0] > 1) or (prob[0] < 0) :
                    raise ValueError("Point Estimate of '%s' should be between 0 and 1" %(self.__name))
            if self.__dist != 'PE':
                if len(prob) != 2:
                    raise IndexError("The distribution of '%s' requires two inputs: Mean/Median and Standard_Deviaton" %(self.__name))
            if uc == False:
                if self.__dist == 'PE':
                    self.__prob = prob*2
                if self.__dist == 'NORM':
                    self.__prob = [numpy.clip(prob[0], 0, 1)]*2
            if uc == True:
                if self.__dist == 'PE':
                    self.__prob = prob*(nsamp+1)
                if self.__dist == 'NORM':
                    self.__prob = [numpy.clip(prob[0], 0, 1)]
                    self.__prob.append(numpy.clip(numpy.random.normal(prob[0],prob[1],nsamp),0,1))


    def __eq__(self, other):
        """
        Test if another Event is equivalent to this node, only the name is considered.

        Input:
            other[str, Event]: The name or Event object to compare against.
        """
        if isinstance(other, str):
            return self.__name == other
        elif isinstance(other, Event):
            return self.__name == other.name
        else:
            raise TypeError("Comparison with between '{}' type is not valid.". \
                            format(type(other).__name__))

    def __str__(self):
        """
        Return a string showing the complete tree beginning at this node.
        """
        return self._dump()

    def __repr__(self):
        """
        Display the name of the node.
        """
        return self.__name

    def _dump(self, level=0):
        """
        Helper for creating a str of the complete tree.
        """
        out = '{}{} (Operator: {})'.format(' '*2*level, self.__name, str(self.__operator))
        for child in self.children:
            out += '\n' + child._dump(level+1) #pylint: disable=protected-access
        return out

class FaultTree(object):
    """
    Creates a fault tree and computes minimal cut sets given a events with type and dependency.

    The minimal cut sets are computed using the MOCUS algorithm as defined by:
    https://www.nrc.gov/docs/ML1119/ML11195A299.pdf

    Inputs:
        events[list]: A list with each row defining an event as follows:

                      'Event0', Event.AND|Event.OR, ['Dep0', 'Dep1', ...]

        events[filename]: A filename containing CSV data representing the events in similar fashion
                          as the list above.

                          Event0, AND|OR, Dep0, Dep1, ...
    """

    def __init__(self, events):

        if isinstance(events, str):
            if not os.path.exists(events):
                raise IOError("The filename {} does not exist.".format(events))
            else:
                self.__events = self.__readEventsFile(events)
        elif not isinstance(events, list):
            raise TypeError("The supplied 'events' must be a filename or a list.")
        else:
            self.__events = events
        # nodes: dictionary of all the events, root: Top Event
        self.__nodes, self.__root = self.__buildTree(self.__events)
        # List of minimal cut sets
        self.__min_cut_sets = self.__computeMiniumCutSets()

    @property
    def logic(self):
        """
        Return the logic of the FaultTree
        """
        return self.__events

    @property
    def root(self):
        """
        Return the root of the FaultTree
        """
        return self.__root

    @property
    def mocus(self):
        """
        Return the minimal cut sets as computed using the MOCUS algorithm.
        """
        return self.__min_cut_sets

    @property
    def nodes(self):
        return self.__nodes

    def __str__(self):
        """
        Print the minimal cut sets.
        """
        return str(self.__min_cut_sets)

    def __repr__(self):
        """
        Display the min cut sets
        """
        return self.__min_cut_sets

    def __computeMiniumCutSets(self):
        """
        Return the minimal cut sets for the given fault tree using the MOCUS method.
        """
        paths = [set([self.__root])]

        done = False
        while not done:
            for node in self.__nodes.itervalues():
                done, paths = self.__mocusUpdatePath(node, paths)
                if not done:
                    break

        # Absorption
        out = []
        for p in paths:
            if p not in out:
                out.append(p)

        # Minimal Cut Sets
        out = FaultTree.__mocusRemoveSubset(out)

        return out

    @staticmethod
    def __buildTree(events):
        """
        Creates a tree given the list of events.
        """
        nodes = dict()
        for name, oper, dep in events:
            node = FaultTree.__getEvent(nodes, name, oper)
            for dname in dep:
                dnode = FaultTree.__getEvent(nodes, dname)
                node.children.append(dnode)
        return nodes, nodes[events[0][0]]

    @staticmethod
    def __getEvent(nodes, name, oper=None):
        """
        Helper for getting or creating a Event object.s
        """
        if name in nodes:
            node = nodes[name]
            if node.operator is None:
                node.operator = oper
        else:
            node = Event(name, oper)
            nodes[name] = node
        return node

    @staticmethod
    def __mocusUpdatePath(node, paths):
        """
        Update the list of paths given a node for the minimal cut set calculation.
        """
        new_paths = []
        done = True
        for row in paths:
            row_changed = False
            for r in row:
                if (r == node) and (r.operator == Event.AND):
                    row.remove(r)
                    new_paths.append(set(r.children + list(row)))
                    row_changed = True
                    done = False
                    break

                elif (r == node) and (r.operator == Event.OR):
                    row.remove(r)
                    for child in r.children:
                        new_paths.append(set([child] + list(row)))
                    row_changed = True
                    done = False
                    break

            if not row_changed:
                new_paths.append(row)
        return done, new_paths

    @staticmethod
    def __mocusRemoveSubset(out):
        """
        Removes cut sets that contain other cut sets as a subset.
        """
        out.sort(key=len)
        out1 = out
        for path in out:
            for o in out:
                if path < o:
                    re = o
                    if re in out1:
                        out1.remove(re)
        return out1

    @staticmethod
    def __readEventsFile(filename):
        """
        Read the events file.
        """
        events = []
        operator = dict(AND=Event.AND, OR=Event.OR)

        # Read the file, it was already tested for existence at this point
        with open(filename) as fid:
            lines = fid.readlines()

        for line in lines:
            items = line.split(',')
            events.append([items[0].strip(),
                           operator[items[1].strip().upper()],
                           [x.strip() for x in items[2:]]])
        return events


class Quantification(object):
    """
    Calculates the minimal cut set probabilities and system level probability
    by propagating basic event probabilities through the fault tree.

    Inputs (9): (1,2,3,4,5,6 are for Fragility inputs). (1,2,3,7,8,9 are for Risk inputs)
        (1) logic[list]: A list with each row defining an event as follows:
                      'Event0', Event.AND|Event.OR, ['Dep0', 'Dep1', ...]

        (1) logic[filename]: A filename containing CSV data representing the events in similar fashion
                         as the list above.
                          Event0, AND|OR, Dep0, Dep1, ...

        (2) bas_events[list]: A list with each row defining a basic event probability as follows:
                                'Basic_event0', 'PE', [Mean]

        (2) bas_events[filename]: A filename containing CSV data representing the basic events in similar fashion
                                as the list above.
                                Basic_event0, PE, Mean

        (3) analysis[string]: Default is "Fragility". The basic event probabilites are assumed to be lognormal fragility parameters.
                                If not "Fragility", the basic event probabilites are assumed to be risk values.

        (4) hazard[list]: A list with each row defining PGA and Probability of Exceedance as follows:
                                [PGA, Prob. of exceedance], [,], ...

                         If None, then the default list is [[0.0608, 0.01], [0.2124, 0.001], [0.4, 0.0001], [0.629, 1e-05], [0.9344, 1e-06], [1.3055, 1e-07]]

        (4) hazard[filename]: A filename containing CSV data representing the hazard data in similar fashion
                                as the list above.
                                PGA, Prob. of exceedance

        (5) IM[list]: Deafult is [0.1,4]. A list with lower and upper bounds of the Intensity Measure.

        (6) nbins[int]: Default is 15. Number of bins for Intensity Measure. Can input any integer greater than 1.

        (7) uncertainty: Default is False (Point Estimate values). If True, performs Monte Carlo simulation.

        (8) nsamp[int]: Default is 1. Number of samples for uncertainty analysis. Can input any integer greater than 1.

        (9) seed[int]: Default is None (set seed from clock). Can input any integer between 0 and (2**32 - 1).
        """

    def __init__(self, logic, bas_events, analysis='Fragility', hazard=None, IM=[0.1, 4], nbins=15, uncertainty=False, nsamp=1, seed=None):

        # create an object 'logic' of class 'FaultTree' by inputting the logic of the fault tree
        self.__logic = FaultTree(logic)
        # import minimal cut sets from object 'logic'
        self.__mcsets = self.__logic.mocus

        # read basic events file
        self.__bas_events = self.__readFile(bas_events)
        # Type of Analysis
        self.__anType = analysis

        # Number of samples
        self.__nsamp = self.__parValue(nsamp,'nsamp')
        # Uncertainty Analysis
        self.__uc = uncertainty
        # set seed for the uncertainty Analysis
        self.__seed = self.__parValue(seed,'seed')

        if self.__anType == "Fragility":
            # read Hazard curve
            if hazard == None:
                self.__hazard = [[0.0608,0.01],[0.2124,0.001],[0.4,0.0001],[0.629,1e-05],[0.9344,1e-06],[1.3055,1e-07]]
            else:
                self.__hazard = self.__readFile(hazard)

            # Intensity Measure range
            if not isinstance(IM, list):
                raise TypeError("The supplied items of IM range must be a list.")
            self.__IM = IM

            # Number of bins for Intensity Measure
            self.__nbins = self.__parValue(nbins,'nbins')
            # list of Intensity Measure bin values
            self.__im = self.__bins(self.__IM,self.__nbins)

            # Interpolating/Exterpolating hazard curve based on Intensity Measure bin values
            self.__haz_freq = self.__hazInterp(self.__hazard,self.__im)

            # dictionary of basic events for fragility input
            self.__Bnodes_Frag = self.__BEprob(self.__bas_events,self.__anType,self.__logic.nodes,self.__im,self.__nsamp,self.__uc,self.__seed)

            # Top event fragility (lognormal parameters and plot)
            #self.__TOPfragility(self)

            # dictionary of basic events risk (convoluting fragility and hazard)
            self.__Bnodes = self.__risk(self.__Bnodes_Frag,self.__haz_freq)
        else:
            self.__im = None
            # dictionary of basic events risk (risk inputs)
            self.__Bnodes = self.__BEprob(self.__bas_events,self.__anType,self.__logic.nodes,self.__im,self.__nsamp,self.__uc,self.__seed)

        # list of lists of minimal cut sets probabilities
        self.__mc_prob = self.__MCprob(self.__Bnodes,self.__mcsets)
        # top event risk
        self.__TOPrisk(self)
        # basic event importance measures
        self.__ImportanceMeasures(self)

    @property
    def logic(self):
        """
        Return the logic of the FaultTree
        """
        return self.__logic.logic

    @property
    def basic_events(self):
        """
        Return basic_event probabilites of the FaultTree
        """
        return self.__bas_events

    @property
    def anType(self):
        """
        Return the analysis type
        """
        return self.__anType

    @property
    def hazard(self):
        """
        Return the hazard curve
        """
        return self.__hazard

    @property
    def IM(self):
        """Return the Intensity Measure range"""
        return self.__IM

    @property
    def nbins(self):
        """Return number of bins"""
        return self.__nbins

    @property
    def uncertainty(self):
        """Return True/False for the uncertainty analysis"""
        return self.__uc

    @property
    def nsamp(self):
        """Return number of samples for the uncertainty analysis"""
        return self.__nsamp

    @property
    def seed(self):
        """Return seed for the uncertainty analysis"""
        return self.__seed

    @property
    def TopRisk(self):
        """Return TOP event Risk"""
        return (self.__TOP_upper_bound[0], self.__TOP_rare[0], self.__TOP_exact[0])

    @property
    def beIMratio(self):
        """Return the basic event ratio importance measures"""
        return (self.__BE_IM['BE_fv'],self.__BE_IM['BE_rrr'],self.__BE_IM['BE_rir'])

    @property
    def beIMdiff(self):
        """Return the basic event difference importance measures"""
        return (self.__BE_IM['BE_rri'],self.__BE_IM['BE_rii'],self.__BE_IM['BE_bi'])

    @staticmethod
    def __BEprob(bas_events,antype,nodes,im,nsamp,uc,seed):
        """
        Function assigns distribution type for Basic_event Nodes and calaulates
        failure probabilities of basic events
        """
        Bnodes = dict()
        for name, dist, probs in bas_events:
            if name in nodes:
                Bnode = nodes[name]
                Bnode.dist = (dist,antype)
                Bnode.prob = (probs,antype,im,uc,nsamp,seed)
                Bnodes[name] = Bnode
            else:
                import warnings
                warnings.warn("Basic event is not used in logic of the fault tree")

                Bnode = Event(name)
                Bnode.dist = (dist, antype)
                Bnode.prob = (probs,antype,im,uc,nsamp,seed)
                Bnodes[name] = Bnode
        return Bnodes

################## Minimal Cut Set Probabilites and Importance Measues ###################

    @staticmethod
    def __MCprob(Bnodes,cutsets):
        """
        Function calculates the probabilities of minimal cut sets
        """
        Min_cut = []
        for i in range(0,len(cutsets)):
            xM = []
            temp = list(cutsets[i])
            for j in range(0,len(temp)):
                xM.append(temp[j].prob)
            temp1 = Quantification.__Intermediate_event(xM,'AND')
            Min_cut.append(temp1)
        return Min_cut

    @staticmethod
    def __MCim(mc_prob,TOP_ub):
        """
        Function calculates the Importance Measures of minimal cut sets
        """
        im=[]
        for i in range(0,len(mc_prob)):
            im.append([100*x/y for x,y in zip(mc_prob[i],TOP_ub)])
        return im

################## Exact probability calculation ##################
    @staticmethod
    def __Min_max(Bnodes,cutsets):
        """
        Function calculates the probability of TOP event using min-max approach (exact)
        """
        import itertools
        sub = []
        for L in range(1, len(cutsets)+1):
          for subset in itertools.combinations(cutsets, L):
            subset = set(list(itertools.chain(*subset)))
            sub.append(list(subset))
        sub_prob = Quantification.__MCprob(Bnodes,sub)
        return Quantification.__Min_exact(sub_prob,cutsets)

    @staticmethod
    def __nCr(n,r):
        """
        Helper function for calculating nCr
        """
        import math
        f = math.factorial
        return f(n) / f(r) / f(n-r)

    @staticmethod
    def __Min_exact(prob,cutsets):
        """
        Helper function for calculating the exact probability of TOP event.
        Summation of all the cutset combinations with appropriate signs.
        """
        cm = []
        for i in range(0,len(cutsets)):
            cm.append(Quantification.__nCr(len(cutsets),i+1))
        ones = []
        for i in range(0,len(cm)):
            if i%2 == 0:
                for j in range(0,int(cm[i])):
                    ones.append([1]*len(prob[0]))
            if i%2 == 1:
                for k in range(0,int(cm[i])):
                    ones.append([-1]*len(prob[0]))
        Sum_all_terms = [[a*b for a,b in zip(x,y)] for x,y in zip(ones,prob)]
        Sum_all_terms = [sum(j) for j in zip(*Sum_all_terms)]
        return Sum_all_terms

################## Basic Event Importance Measures ###################
### Ratio Importance    : Fussel-Vesely Importance, Risk Reduction Ratio, Risk Increase Ratio
### Interval Importance : Risk Reduction Interval,Risk Increase Interval, Birnbaum Importance

    @staticmethod
    def __BEim(mcsets,mc_prob,TOP_ub,Bnodes,bas_events,MCprob,IE,nsamp,uc,seed,stat,antype,im):
        import collections
        BE_IM = collections.OrderedDict([('BE_fv', []), ('BE_rrr', []), ('BE_rir', []), ('BE_rri', []), ('BE_rii', []), ('BE_bi', [])])
        BE_count = []
        BE_index = []
        BE_fv = []
        BE_rrr = []
        BE_rir = []
        BE_rri = []
        BE_rii = []
        BE_bi = []
        for i,key in enumerate(bas_events):
            prob = Bnodes[key[0]].prob
            temp = []
            # get the count of basic event occurences in minimal cut sets
            count = sum(list(x).count(key[0]) for x in mcsets)
            BE_count.append(count)
            for j in range(0,len(mcsets)):
                if key[0] in list(mcsets[j]):
                    temp.append(j)
            # get the index position of basic event in mimimal cut sets
            BE_index.append(temp)

            # F1: minimal cut set upper bound evaluated with basic event probability set to one
            Bnodes[key[0]].dist = ('PE',"risk")
            Bnodes[key[0]].prob = ([1],"risk",im,uc,nsamp,seed)
            mcProb1 = MCprob(Bnodes,mcsets)
            F1 = IE(mcProb1,'OR')

            # F0: minimal cut set upper bound evaluated with basic event probability set to zero
            Bnodes[key[0]].prob = ([0],"risk",im,uc,nsamp,seed)
            mcProb0 = MCprob(Bnodes,mcsets)
            F0 = IE(mcProb0,'OR')

            import numpy
            if count != 0:
                temp1 = [mc_prob[item] for item in BE_index[i]]
                # Fi: minimal cut set upper bound evaluated for all minimal cut sets containing i'th basic event
                Fi = IE(temp1,'OR')
                # Ratio and Interval Importance Measures
                BE_fv.append([x/y for x,y in zip(Fi,TOP_ub)])
                BE_rrr.append([y/x if numpy.any(x!=0) else float('Inf') for x,y in zip(F0,TOP_ub)])
                BE_rri.append([y-x for x,y in zip(F0,TOP_ub)])
                BE_rir.append([x/y for x,y in zip(F1,TOP_ub)])
                BE_rii.append([x-y for x,y in zip(F1,TOP_ub)])
                BE_bi.append([x-y for x,y in zip(F1,F0)])
            else:
                BE_fv.append([0,[0]])
                BE_rrr.append([0,[0]])
                BE_rri.append([0,[0]])
                BE_rir.append([0,[0]])
                BE_rii.append([0,[0]])
                BE_bi.append([0,[0]])

            if antype == 'Fragility':
                Bnodes[key[0]].dist = ('PE',"risk")
                Bnodes[key[0]].prob = ([prob[0]],"risk",im,uc,nsamp,seed)
            else:
                Bnodes[key[0]].dist = (key[1],"risk")
                Bnodes[key[0]].prob = (key[2],"risk",im,uc,nsamp,seed)

        #### Statistics of importance measures
        BE_IM['BE_fv'] = stat(BE_fv)
        BE_IM['BE_rrr'] = stat(BE_rrr)
        BE_IM['BE_rir'] = stat(BE_rir)
        BE_IM['BE_rri'] = stat(BE_rri)
        BE_IM['BE_rii'] = stat(BE_rii)
        BE_IM['BE_bi'] = stat(BE_bi)
        return BE_count,BE_IM

    @staticmethod
    def __IMstat(IM):
        """
        Function calculates the statistics of Importance measures
        """
        import numpy as np
        IM_stat = []
        for i in range(0,len(IM)):
            a = []
            ## Point Estimate (PE)
            a.append(IM[i][0])
            ## Mean
            a.append(np.mean(IM[i][1]))
            ## Median
            a.append(np.median(IM[i][1]))
            ## 5th percentile
            a.append(np.percentile(IM[i][1],5))
            ## 95th percentile
            a.append(np.percentile(IM[i][1],95))
            ## SD
            a.append(np.std(IM[i][1]))
            IM_stat.append(a)
        return IM_stat


##############################################################################

    @staticmethod
    def __Intermediate_event(dep_events,gate):
        """
        Function calculates the probability of intersection/union of sets
        """
        from operator import mul,sub
        if gate == 'AND':
            return [reduce(mul, x) for x in zip(*dep_events)]
        if gate == 'OR':
            temp = [reduce(mul, map(sub,[1]*len(x),x)) for x in zip(*dep_events)]
            return [x-y for x,y in zip([1]*len(temp),temp)]

    @staticmethod
    def __bins(IM,nbins):
        """
        Function for getting BIN mean values of Intensity Measure
        """
        im = []
        import numpy as np
        temp = np.linspace(IM[0],IM[1],nbins+1)
        for i in range(0,len(temp)-1):
            im.append(0.5*(temp[i]+temp[i+1]))
        return im

    @staticmethod
    def __lnparameters(im,fragility):
        """
        Function for getting the lognormal parameters - median and sd
        using least square fit
        """
        from scipy.optimize import curve_fit

        (logpara,logcov) = curve_fit(Quantification.__logcdf,im,fragility)
        return logpara


    @staticmethod
    def __logcdf(x, Am, beta):
        """
        Function for fitting conditional probabilities into a lognormal cdf
        """
        from scipy.stats import lognorm
        return lognorm.cdf(x, beta, loc=0, scale=Am)

    @staticmethod
    def __frag_plot(im,fragility,para):
        """
        Plot the TOP Event fragility (data points and least square fit)
        """
        import matplotlib.pyplot as plt
        from scipy.stats import lognorm
        #plot data points
        plt.plot(im, fragility, 'r.', label='Data points', linewidth=2.5)
        #plot points from least square fit parameters
        plt.plot(im,lognorm.cdf(im,para[1],loc=0,scale=para[0]),'g-',label='Least Square Fit')
        # plot the lines corresponing to meadian value
        x1, y1 = [para[0], para[0]], [0, 0.5]
        x2, y2 = [0,para[0]],[0.5, 0.5]
        plt.plot(x1, y1,'b--',marker = 'o')
        plt.plot(x2, y2, 'b--',marker = 'o',label = "Median: %f" %(para[0]))
        #plot parameters
        plt.legend(loc='upper left')
        plt.xlabel('Intensity Measure')
        plt.ylabel('Fragility')
        plt.title('TOP Event Fragility')
        plt.grid(True)
        return plt.show()

    @staticmethod
    def __hazInterp(haz,im):
        """
        Function for interpolating the hazard curve based on range of intensity measures
        """
        # hazard curve x-axis (Intensity Measure)
        haz_IM = zip(*haz)[0]
        # hazard curve y-axis (Prob. of exceedance)
        haz_freq = zip(*haz)[1]

        from scipy import interpolate
        import numpy as np
        yinterp = interpolate.interp1d(np.log10(haz_IM), np.log10(haz_freq),fill_value='extrapolate')
        Yinterp = 10**yinterp(np.log10(im))
        # Plot Hazard Curve
        # fig, ax = plt.subplots()
        # ax.loglog(haz_IM,haz_freq)
        # ax.grid()
        # ax.loglog(im,Yinterp,'ro')
        # plt.show()
        return Yinterp

    @staticmethod
    def __risk(Bnodes,haz):
        """
        Function for calculating risk by convoluting hazard and fragility
        Assign risk to basic event probabilites
        """
        for key in Bnodes:
            risk = [sum([x*y for x,y in zip(Bnodes[key].prob,haz)])]
            Bnodes[key].dist = ("PE","risk")
            Bnodes[key].prob = (risk,"risk",None,False,1,None)
        return Bnodes

    @staticmethod
    def __parValue(value,name):
        """
        Function for raising input errors (nbins,nsamp,seed)
        """
        if name == 'seed':
            if value != None:
                if not isinstance(value, int):
                    raise TypeError("The supplied value of %s must be an integer." %(name))
                if (value<0) or (value>2**32-1):
                    raise ValueError("The supplied value of %s must be a positive integer." %(name))
        else:
            if not isinstance(value, int):
                raise TypeError("The supplied value of %s must be an integer." %(name))
            if value <= 0:
                raise ValueError("The supplied value of %s must be a positive integer." %(name))
        return value


    @staticmethod
    def __readFile(fname):
        """
        Function for reading the BasicEvents/Hazard file.
        """
        if isinstance(fname, str):
            if not os.path.exists(fname):
                raise IOError("The filename {} does not exist.".format(fname))
            else:
                data = Quantification.__readEventsFile(fname)
        elif isinstance(fname, list):
            data = fname
        else:
            raise TypeError(
                "The supplied {} must be a filename or a list.".format(fname))
        return data

    @staticmethod
    def __readEventsFile(filename):
        """
        Helper function for Read file.
        """
        data = []
        with open(filename) as fid:
            lines = fid.readlines()
        for line in lines:
            items = line.split(',')
            if len(items) > 2:
                data.append([items[0].strip(),
                           items[1].strip().upper(),
                           [float(x.strip()) for x in items[2:]]])
            else:
                data.append([float(items[0].strip()),float(items[1].strip())])
        return data

    @staticmethod
    def __TOPfragility(self):
        """
        Function for top event fragility.
        """
        # calculate TOP event fragility using min-max approach
        self.__TOP_Frag = self.__Min_max(self.__Bnodes_Frag,self.__mcsets)
        # lognormal parameters of TOP Event fragility
        self.__ln = self.__lnparameters(self.__im,self.__TOP_Frag)
        # TOP event Fragility plot
        self.__Frag_plot = self.__frag_plot(self.__im,self.__TOP_Frag,self.__ln)

    @staticmethod
    def __TOPrisk(self):
        """
        Function for top event risk.
        """
        self.__TOP_rare = [sum(i) for i in zip(*self.__mc_prob)]
        self.__TOP_rare_stat = self.__IMstat([self.__TOP_rare])
            # 2. Upper Bound Approximation
        self.__TOP_upper_bound = self.__Intermediate_event(self.__mc_prob,'OR')
        self.__TOP_ub_stat = self.__IMstat([self.__TOP_upper_bound])
            # 3. Min-Max approach
        self.__TOP_exact = self.__Min_max(self.__Bnodes,self.__mcsets)
        self.__TOP_exact_stat = self.__IMstat([self.__TOP_exact])

    @staticmethod
    def __ImportanceMeasures(self):
        """
        Function for importance measures.
        """
        import collections
        self.__TOP_cal = collections.OrderedDict([('MCS Upper Bound', self.__TOP_ub_stat) \
        ,('Rare Event',self.__TOP_rare_stat),('Min Max',self.__TOP_exact_stat)])

        # minimal cut set Fussel-Vesely Important Measures
        self.__mc_im = self.__MCim(self.__mc_prob,self.__TOP_upper_bound)

        ################## Basic event Important Measures #########################
        self.__count,self.__BE_IM = self.__BEim(self.__mcsets \
        ,self.__mc_prob,self.__TOP_upper_bound,self.__Bnodes,self.__bas_events,self.__MCprob,self.__Intermediate_event \
        ,self.__nsamp,self.__uc,self.__seed,self.__IMstat,self.__anType,self.__im)

        self.__res = self.__results(self)

    @staticmethod
    def __results(self):
        """
        Function for writing results in csv format.
        """
        import csv
        with open('Cut Sets.csv', 'w') as f1:
            writer = csv.writer(f1, delimiter=',', lineterminator='\n',)
            writer.writerows([['Cut Sets', 'Prob/Freq', 'IM (%)'],
                              ['Total', self.__TOP_upper_bound[0], '100']])
            for i in range(0, len(self.__mcsets)):
                writer.writerow([str(list(self.__mcsets[i])), self.__mc_prob[i][0], self.__mc_im[i][0]])

        with open('Top Event.csv', 'w') as f2:
            writer = csv.writer(f2, delimiter=',', lineterminator='\n',)
            writer.writerows([['Quantification Method', 'Prob/Freq', 'Mean','Median','5th','95th','SD']])
            for i, keyT in enumerate(self.__TOP_cal):
                row = [keyT] + [self.__TOP_cal[keyT][0][k] for k in range(0, len(self.__TOP_cal[keyT][0]))]
                writer.writerow(row)

        with open('Importance Measures.csv', 'w') as f3:
            writer = csv.writer(f3, delimiter=',', lineterminator='\n',)
            writer.writerows([['Basic Event', 'Count', 'Prob','FV','RRR','RIR','RRI','RII','BI']])
            for i, keyb in enumerate(self.__bas_events):
                row = [keyb[0]] + [self.__count[i]] + \
                    [self.__Bnodes[keyb[0]].prob[0]] + [self.__BE_IM[key][i][0] for key in self.__BE_IM]
                writer.writerow(row)

        wname = ['FV', 'RRR', 'RIR', 'RRI', 'RII', 'BI']
        for i, key in enumerate(self.__BE_IM):
            with open(wname[i]+'.csv', 'w') as f4:
                writer = csv.writer(f4, delimiter=',', lineterminator='\n',)
                writer.writerows([['Basic Event', 'PE', 'Mean','Median','5th','95th','SD']])
                for j, keyb in enumerate(self.__bas_events):
                    row = [keyb[0]] + [self.__BE_IM[key][j][k] for k in range(0, len(self.__BE_IM[key][0]))]
                    writer.writerow(row)

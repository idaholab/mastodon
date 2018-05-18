#!/usr/bin/env python
"""
Provides FaultTree class for creating fault trees and minimum cutsets, given the logic of a
fault tree and Quantification class for calculating minimal cutset probabilities and system
level probability, given the basic event probabilities.
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

    def __init__(self, name, operator=None):
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
    def dist(self, dist):
        """Set the distribution attribute"""
        distribution = dist[0]
        antype = dist[1]
        if self.__operator is not None:
            raise AttributeError("'%s' is not a basic node (i.e., the operator attribute is "
                                 "set), therefore it can not have distribution." % (self.__name))
        if antype == 'Fragility':
            if distribution not in ['LNORM']:
                raise TypeError("The dist. of '%s' for fragility must be "
                                "one of the following: LNORM" % (self.__name))
        else:
            if distribution not in ['PE', 'NORM']:
                raise TypeError("The dist. of '%s' for risk must be "
                                "one of the following: PE, NORM" % (self.__name))
        self.__dist = distribution

    @property
    def prob(self):
        """Return the probability attribute"""
        return self.__prob

    @prob.setter
    def prob(self, values):
        """Set the probability attribute"""
        self.__prob = []
        prob = values[0]
        antype = values[1]
        im = values[2]
        uc = values[3]
        nsamp = values[4]
        seed = values[5]
        if not isinstance(prob, list):
            raise TypeError("The supplied items of '%s' must be a list." % (self.__name))

        import numpy
        import scipy.stats
        numpy.random.seed(seed)
        if antype == "Fragility":
            if self.__dist == 'LNORM':
                if len(prob) != 2:
                    raise IndexError("The distribution of '%s' requires two inputs: "
                                     "Mean/Median and Standard_Deviaton" % (self.__name))
                for i in range(0, len(im)):
                    self.__prob.append(scipy.stats.norm.cdf(numpy.log(im[i]/prob[0])/prob[1]))
        else:
            if self.__dist == 'PE':
                if len(prob) != 1:
                    raise IndexError("Point Estimate of '%s' requires only one input: "
                                     "Mean/Median probability" % (self.__name))
                if (prob[0] > 1) or (prob[0] < 0):
                    raise ValueError("PE of '%s' should be between 0 & 1" % (self.__name))
            if self.__dist != 'PE':
                if len(prob) != 2:
                    raise IndexError("The distribution of '%s' requires two inputs: "
                                     "Mean/Median and Standard_Deviaton" % (self.__name))
            if not uc:
                if self.__dist == 'PE':
                    self.__prob = prob*2
                if self.__dist == 'NORM':
                    self.__prob = [numpy.clip(prob[0], 0, 1)]*2
            if uc:
                if self.__dist == 'PE':
                    self.__prob = prob*(nsamp+1)
                if self.__dist == 'NORM':
                    self.__prob = [numpy.clip(prob[0], 0, 1)]
                    self.__prob.append(
                        numpy.clip(numpy.random.normal(prob[0], prob[1], nsamp), 0, 1))

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
            raise TypeError("Comparison with between '{}' type is not valid.".
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
            out += '\n' + child._dump(level+1)  # pylint: disable=protected-access
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
        """
        Return nodes
        """
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

    (1) logic[filename]: A filename containing CSV data representing the events
                             in similar fashion as the list above.
                        Event0, AND|OR, Dep0, Dep1, ...

    (2) bas_events[list]: A list with each row defining a basic event probability as follows:
                        'Basic_event0', 'PE', [Mean]

    (2) bas_events[filename]: A filename containing CSV data representing the basic events
                                  in similar fashion as the list above.
                        Basic_event0, PE, Mean

    (3) analysis[string]: Default is "Fragility". The basic event prob. are assumed to be
                                lognormal fragility parameters.
                           If not "Fragility", the basic event prob. are assumed to be risk values.

    (4) hazard[list]: A list with each row defining PGA and Prob. of Exceedance as follows:
                            [PGA, Prob. of exceedance], [,], ...

                        If None, then the default list is [[0.0608, 0.01], [0.2124, 0.001],
                            [0.4, 0.0001], [0.629, 1e-05], [0.9344, 1e-06], [1.3055, 1e-07]]

    (4) hazard[filename]: A filename containing CSV data representing the hazard data in
                                similar fashion as the list above.
                            PGA, Prob. of exceedance

    (5) IM[list]: Deafult is [0.1,4]. A list with lower & upper bounds of Intensity Measure.

    (6) nbins[int]: Default is 15. Number of bins for Intensity Measure.
                        Can input any integer greater than 1.

    (7) uncertainty: Default is False (Point Estimate values).
                        If True, performs Monte Carlo simulation.

    (8) nsamp[int]: Default is 1. Number of samples for uncertainty analysis.
                        Can input any integer greater than 1.

    (9) seed[int]: Default is None (set seed from clock).
                    Can input any integer between 0 and (2**32 - 1).
    """

    def __init__(self, logic, bas_events, analysis='Fragility', hazard=None, IM=None,
                 nbins=15, uncertainty=False, nsamp=1, seed=None):

        # create an object 'logic' of class 'FaultTree' by inputting the logic of the fault tree
        self.__logic = FaultTree(logic)
        # import minimal cut sets from object 'logic'
        self.__mcsets = self.__logic.mocus

        # read basic events file
        self.__bas_events = self.__readFile(bas_events)
        # Type of Analysis
        self.__antype = analysis

        # Number of samples
        self.__nsamp = self.__parValue(nsamp, 'nsamp')
        # Uncertainty Analysis
        self.__uc = uncertainty
        # set seed for the uncertainty Analysis
        self.__seed = self.__parValue(seed, 'seed')

        if self.__antype == "Fragility":
            # read Hazard curve
            if hazard is None:
                self.__hazard = [[0.0608, 0.01], [0.2124, 0.001], [0.4, 0.0001],
                                 [0.629, 1e-05], [0.9344, 1e-06], [1.3055, 1e-07]]
            else:
                self.__hazard = self.__readFile(hazard)

            # Intensity Measure range
            if IM is None:
                self.__imrang = [0.1, 4]
            else:
                if not isinstance(IM, list):
                    raise TypeError("The supplied items of IM range must be a list.")
                self.__imrang = IM

            # Number of bins for Intensity Measure
            self.__nbins = self.__parValue(nbins, 'nbins')
            # list of Intensity Measure bin values
            self.__im = self.__bins(self.__imrang, self.__nbins)

            # Interpolating/Exterpolating hazard curve based on Intensity Measure bin values
            self.__haz_freq = self.__hazInterp(self.__hazard, self.__im)

            # dictionary of basic events for fragility input
            self.__bnodes_frag = self.__BEprob(self.__bas_events, self.__antype, self.__logic.nodes,
                                               self.__im, self.__nsamp, self.__uc, self.__seed)

            # Top event fragility (lognormal parameters and plot)
            self.__topfragility, self.__ln = self.__TOPfragility(
                self.__Min_max, self.__mcsets, self.__MCprob,
                self.__lnparameters, self.__im
            )

            # dictionary of basic events risk (convoluting fragility and hazard)
            self.__bnodes = self.__risk(self.__bnodes_frag, self.__haz_freq)
        else:
            self.__im = None
            # dictionary of basic events risk (risk inputs)
            self.__bnodes = self.__BEprob(self.__bas_events, self.__antype, self.__logic.nodes,
                                          self.__im, self.__nsamp, self.__uc, self.__seed)

        # list of lists of minimal cut sets probabilities
        self.__mc_prob = self.__MCprob(self.__mcsets)

        # top event risk
        # 1. Rare event approximation
        self.__top_rare = [sum(i) for i in zip(*self.__mc_prob)]
        self.__top_rare_stat = self.__IMstat([self.__top_rare])
        # 2. Upper Bound Approximation
        self.__top_upper_bound = self.__Intermediate_event(self.__mc_prob, 'OR')
        self.__top_ub_stat = self.__IMstat([self.__top_upper_bound])
        # 3. Min-Max approach
        self.__top_exact = self.__Min_max(self.__mcsets, self.__MCprob)
        self.__top_exact_stat = self.__IMstat([self.__top_exact])

        # Importance Measures
        import collections
        self.__top_cal = collections.OrderedDict([('MCS Upper Bound', self.__top_ub_stat),
                                                  ('Rare Event', self.__top_rare_stat),
                                                  ('Min Max', self.__top_exact_stat)])
        # minimal cut set Fussel-Vesely Important Measures
        self.__mc_im = self.__MCim(self.__mc_prob, self.__top_upper_bound)
        # basic event Importance Measures
        self.__count, self.__be_im = self.__BEim(
            self.__mcsets, self.__mc_prob, self.__top_upper_bound,
            self.__bnodes, self.__bas_events, self.__MCprob,
            self.__Intermediate_event, self.__nsamp, self.__uc, self.__seed,
            self.__IMstat, self.__antype, self.__im)

        # Results to csv format
        self.__results(
            self.__top_upper_bound, self.__mcsets, self.__mc_prob, self.__mc_im,
            self.__top_cal, self.__bas_events, self.__count, self.__bnodes, self.__be_im
        )

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
    def antype(self):
        """
        Return the analysis type
        """
        return self.__antype

    @property
    def hazard(self):
        """
        Return the hazard curve
        """
        return self.__hazard

    @property
    def imrang(self):
        """Return the Intensity Measure range"""
        return self.__imrang

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
    def toprisk(self):
        """Return TOP event Risk"""
        return (self.__top_upper_bound[0], self.__top_rare[0], self.__top_exact[0])

    @property
    def be_im_ratio(self):
        """Return the basic event ratio importance measures"""
        return (self.__be_im['BE_fv'], self.__be_im['BE_rrr'], self.__be_im['BE_rir'])

    @property
    def be_im_diff(self):
        """Return the basic event difference importance measures"""
        return (self.__be_im['BE_rri'], self.__be_im['BE_rii'], self.__be_im['BE_bi'])

    @staticmethod
    def __BEprob(bas_events, antype, nodes, intmes, nsamp, uncert, seed):
        """
        Function assigns distribution type for Basic_event Nodes and calaulates
        failure probabilities of basic events
        """
        bnodes = dict()
        for name, dist, probs in bas_events:
            if name in nodes:
                bnode = nodes[name]
                bnode.dist = (dist, antype)
                bnode.prob = (probs, antype, intmes, uncert, nsamp, seed)
                bnodes[name] = bnode
            else:
                import warnings
                warnings.warn("Basic event is not used in logic of the fault tree")

                bnode = Event(name)
                bnode.dist = (dist, antype)
                bnode.prob = (probs, antype, intmes, uncert, nsamp, seed)
                bnodes[name] = bnode
        return bnodes

################## Minimal Cut Set Probabilites and Importance Measues ###################

    @staticmethod
    def __MCprob(cutsets):
        """
        Function calculates the probabilities of minimal cut sets
        """
        min_cut = []
        for i in range(0, len(cutsets)):
            xm = []
            temp = list(cutsets[i])
            for j in range(0, len(temp)):
                xm.append(temp[j].prob)
            temp1 = Quantification.__Intermediate_event(xm, 'AND')
            min_cut.append(temp1)
        return min_cut

    @staticmethod
    def __MCim(mc_prob, top_ub):
        """
        Function calculates the Importance Measures of minimal cut sets
        """
        im = []
        for i in range(0, len(mc_prob)):
            im.append([100*x/y for x, y in zip(mc_prob[i], top_ub)])
        return im

################## Exact probability calculation ##################
    @staticmethod
    def __Min_max(cutsets, mcprob):
        """
        Function calculates the probability of TOP event using min-max approach (exact)
        """
        import itertools
        sub = []
        for l in range(1, len(cutsets)+1):
            for subset in itertools.combinations(cutsets, l):
                subset = set(list(itertools.chain(*subset)))
                sub.append(list(subset))
        sub_prob = mcprob(sub)
        return Quantification.__Min_exact(sub_prob, cutsets)

    @staticmethod
    def __nCr(nval, rval):
        """
        Helper function for calculating nCr
        """
        import math
        f = math.factorial
        return f(nval) / f(rval) / f(nval-rval)

    @staticmethod
    def __Min_exact(prob, cutsets):
        """
        Helper function for calculating the exact probability of TOP event.
        Summation of all the cutset combinations with appropriate signs.
        """
        cm = []
        for i in range(0, len(cutsets)):
            cm.append(Quantification.__nCr(len(cutsets), i+1))
        ones = []
        for i in range(0, len(cm)):
            if i%2 == 0:
                for j in range(0, int(cm[i])):
                    ones.append([1]*len(prob[0]))
            if i%2 == 1:
                for _ in range(0, int(cm[i])):
                    ones.append([-1]*len(prob[0]))
        sum_all_terms = [[a*b for a, b in zip(x, y)] for x, y in zip(ones, prob)]
        sum_all_terms = [sum(j) for j in zip(*sum_all_terms)]
        return sum_all_terms

################## Basic Event Importance Measures ###################
### Ratio Importance    : Fussel-Vesely Importance, Risk Reduction Ratio, Risk Increase Ratio
### Interval Importance : Risk Reduction Interval,Risk Increase Interval, Birnbaum Importance

    @staticmethod
    def __BEim(mcsets, mc_prob, top_ub, bnodes, bas_events, mcprob, int_evnt,
               nsamp, uncert, seed, stat, antype, intmes):
        import collections
        be_im = collections.OrderedDict([('BE_fv', []), ('BE_rrr', []), ('BE_rir', []),
                                         ('BE_rri', []), ('BE_rii', []), ('BE_bi', [])])
        be_count = []
        be_index = []
        be_fv = []
        be_rrr = []
        be_rir = []
        be_rri = []
        be_rii = []
        be_bi = []
        for i, key in enumerate(bas_events):
            prob = bnodes[key[0]].prob
            temp = []
            # get the count of basic event occurences in minimal cut sets
            count = sum(list(x).count(key[0]) for x in mcsets)
            be_count.append(count)
            for j in range(0, len(mcsets)):
                if key[0] in list(mcsets[j]):
                    temp.append(j)
            # get the index position of basic event in mimimal cut sets
            be_index.append(temp)

            # F1: minimal cut set upper bound evaluated with basic event probability set to one
            bnodes[key[0]].dist = ('PE', "risk")
            bnodes[key[0]].prob = ([1], "risk", intmes, uncert, nsamp, seed)
            mc_prob1 = mcprob(mcsets)
            f1 = int_evnt(mc_prob1, 'OR')

            # F0: minimal cut set upper bound evaluated with basic event probability set to zero
            bnodes[key[0]].prob = ([0], "risk", intmes, uncert, nsamp, seed)
            mc_prob0 = mcprob(mcsets)
            f0 = int_evnt(mc_prob0, 'OR')

            if count != 0:
                temp1 = [mc_prob[item] for item in be_index[i]]
                # Fi: MCS upper bound evaluated for all MCS containing i'th basic event
                fi = int_evnt(temp1, 'OR')
                # Ratio and Interval Importance Measures
                be_fv.append([x/y for x, y in zip(fi, top_ub)])
                be_rrr.append([y/x for x, y in zip(f0, top_ub)])
                be_rri.append([y-x for x, y in zip(f0, top_ub)])
                be_rir.append([x/y for x, y in zip(f1, top_ub)])
                be_rii.append([x-y for x, y in zip(f1, top_ub)])
                be_bi.append([x-y for x, y in zip(f1, f0)])
            else:
                be_fv.append([0, [0]])
                be_rrr.append([0, [0]])
                be_rri.append([0, [0]])
                be_rir.append([0, [0]])
                be_rii.append([0, [0]])
                be_bi.append([0, [0]])

            if antype == 'Fragility':
                bnodes[key[0]].dist = ('PE', "risk")
                bnodes[key[0]].prob = ([prob[0]], "risk", intmes, uncert, nsamp, seed)
            else:
                bnodes[key[0]].dist = (key[1], "risk")
                bnodes[key[0]].prob = (key[2], "risk", intmes, uncert, nsamp, seed)

        #### Statistics of importance measures
        be_im['BE_fv'] = stat(be_fv)
        be_im['BE_rrr'] = stat(be_rrr)
        be_im['BE_rir'] = stat(be_rir)
        be_im['BE_rri'] = stat(be_rri)
        be_im['BE_rii'] = stat(be_rii)
        be_im['BE_bi'] = stat(be_bi)
        return be_count, be_im

    @staticmethod
    def __IMstat(impmes):
        """
        Function calculates the statistics of Importance measures
        """
        import numpy as np
        im_stat = []
        for i in range(0, len(impmes)):
            imp_array = impmes[i][1]
            if not isinstance(imp_array, list):
                imp_array = np.array([imp_array])
            imp_array[np.isinf(imp_array)] = np.nan
            a = []
            ## Point Estimate (PE)
            a.append(impmes[i][0])
            ## Mean
            a.append(np.nanmean(imp_array))
            ## Median
            a.append(np.nanmedian(imp_array))
            ## 5th percentile
            a.append(np.nanpercentile(imp_array, 5))
            ## 95th percentile
            a.append(np.nanpercentile(imp_array, 95))
            ## SD
            a.append(np.nanstd(imp_array))
            im_stat.append(a)
        return im_stat

##############################################################################

    @staticmethod
    def __Intermediate_event(dep_events, gate):
        """
        Function calculates the probability of intersection/union of sets
        """
        from operator import mul, sub
        if gate == 'AND':
            return [reduce(mul, x) for x in zip(*dep_events)]
        if gate == 'OR':
            temp = [reduce(mul, map(sub, [1]*len(x), x)) for x in zip(*dep_events)]
            return [x-y for x, y in zip([1]*len(temp), temp)]

    @staticmethod
    def __bins(imrange, nbins):
        """
        Function for getting BIN mean values of Intensity Measure
        """
        im = []
        import numpy as np
        temp = np.linspace(imrange[0], imrange[1], nbins+1)
        for i in range(0, len(temp)-1):
            im.append(0.5*(temp[i]+temp[i+1]))
        return im

    @staticmethod
    def __lnparameters(intmes, fragility):
        """
        Function for getting the lognormal parameters - median and sd
        using least square fit
        """
        from scipy.optimize import curve_fit

        (logpara, logcov) = curve_fit(Quantification.__logcdf, intmes, fragility)
        return logpara, logcov


    @staticmethod
    def __logcdf(intmes, amedian, beta):
        """
        Function for fitting conditional probabilities into a lognormal cdf
        """
        from scipy.stats import lognorm
        return lognorm.cdf(intmes, beta, loc=0, scale=amedian)

    @staticmethod
    def __hazInterp(haz, intmes):
        """
        Function for interpolating the hazard curve based on range of intensity measures
        """
        # hazard curve x-axis (Intensity Measure)
        haz_im = zip(*haz)[0]
        # hazard curve y-axis (Prob. of exceedance)
        haz_freq = zip(*haz)[1]

        from scipy import interpolate
        import numpy as np
        yinterp = interpolate.interp1d(np.log10(haz_im), np.log10(haz_freq),
                                       fill_value='extrapolate')
        y_interp = 10**yinterp(np.log10(intmes))
        # Plot Hazard Curve
        # fig, ax = plt.subplots()
        # ax.loglog(haz_IM,haz_freq)
        # ax.grid()
        # ax.loglog(im,Yinterp,'ro')
        # plt.show()
        return y_interp

    @staticmethod
    def __risk(bnodes, haz):
        """
        Function for calculating risk by convoluting hazard and fragility
        Assign risk to basic event probabilites
        """
        for key in bnodes:
            risk = [sum([x*y for x, y in zip(bnodes[key].prob, haz)])]
            bnodes[key].dist = ("PE", "risk")
            bnodes[key].prob = (risk, "risk", None, False, 1, None)
        return bnodes

    @staticmethod
    def __parValue(value, name):
        """
        Function for raising input errors (nbins,nsamp,seed)
        """
        if name == 'seed':
            if value != None:
                if not isinstance(value, int):
                    raise TypeError("The supplied value of %s must be an integer." % (name))
                if (value < 0) or (value > 2**32-1):
                    raise ValueError("The supplied value of %s must be a +ve integer." % (name))
        else:
            if not isinstance(value, int):
                raise TypeError("The supplied value of %s must be an integer." % (name))
            if value <= 0:
                raise ValueError("The supplied value of %s must be a +ve integer." % (name))
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
                data.append([float(items[0].strip()), float(items[1].strip())])
        return data

    @staticmethod
    def __TOPfragility(min_max, mcsets, mcprob, lnparameters, intmes):
        """
        Function for top event fragility.
        """
        # calculate TOP event fragility using min-max approach
        top_frag = min_max(mcsets, mcprob)
        # lognormal parameters of TOP Event fragility
        lnpar = lnparameters(intmes, top_frag)
        return top_frag, lnpar[0]

    @staticmethod
    def __results(top_upper_bound, mcsets, mc_prob, mc_im, top_cal, bas_events, count,
                  bnodes, be_im):
        """
        Function for writing results in csv format.
        """
        import csv
        with open('Cut Sets.csv', 'w') as f1:
            writer = csv.writer(f1, delimiter=',', lineterminator='\n',)
            writer.writerows([['Cut Sets', 'Prob/Freq', 'IM (%)'],
                              ['Total', top_upper_bound[0], '100']])
            for i in range(0, len(mcsets)):
                writer.writerow([str(list(mcsets[i])), mc_prob[i][0],
                                 mc_im[i][0]])

        with open('Top Event.csv', 'w') as f2:
            writer = csv.writer(f2, delimiter=',', lineterminator='\n',)
            writer.writerows([['Quantification Method', 'Prob/Freq', 'Mean',
                               'Median', '5th', '95th', 'SD']])
            for i, keyt in enumerate(top_cal):
                row = [keyt] + [top_cal[keyt][0][k]
                                for k in range(0, len(top_cal[keyt][0]))]
                writer.writerow(row)

        with open('Importance Measures.csv', 'w') as f3:
            writer = csv.writer(f3, delimiter=',', lineterminator='\n',)
            writer.writerows([['Basic Event', 'Count', 'Prob', 'FV', 'RRR',
                               'RIR', 'RRI', 'RII', 'BI']])
            for i, keyb in enumerate(bas_events):
                row = [keyb[0]] + [count[i]] + \
                    [bnodes[keyb[0]].prob[0]] + [be_im[key][i][0] for key in be_im]
                writer.writerow(row)

        wname = ['FV', 'RRR', 'RIR', 'RRI', 'RII', 'BI']
        for i, key in enumerate(be_im):
            with open(wname[i]+'.csv', 'w') as f4:
                writer = csv.writer(f4, delimiter=',', lineterminator='\n',)
                writer.writerows([['Basic Event', 'PE', 'Mean', 'Median', '5th', '95th', 'SD']])
                for j, keyb in enumerate(bas_events):
                    row = [keyb[0]] + [be_im[key][j][k]
                                       for k in range(0, len(be_im[key][0]))]
                    writer.writerow(row)

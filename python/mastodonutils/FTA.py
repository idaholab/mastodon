#!/usr/bin/env python

"""
Provides three classes: Event, FaultTree, and Quantification.
An object of the Event class is an event, which could be a basic event,
intermediate event, or a top event. A FaultTree object is composed of several
events in a logic tree representation. The Quantification class uses the
Event and FaultTree classes to perform a fault tree analysis resulting in
minimal cutsets, minimal cutset probabilities, system level probability,
importance measures, and the system risk of unacceptable performance. The
description of each individual class and their usage is below.
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
            if distribution not in ['LNORM', 'PE']:
                raise TypeError("The dist. of '%s' for fragility must be "
                                "one of the following: LNORM, PE" % (self.__name))
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
            if self.__dist == 'PE':
                if len(prob) != 1:
                    raise IndexError("Point Estimate of '%s' requires only one input: "
                                     "Mean/Median probability" % (self.__name))
                    if (prob[0] > 1) or (prob[0] < 0):
                        raise ValueError("PE of '%s' should be between 0 & 1" % (self.__name))
                self.__prob = prob*len(im)
            if uc:
                raise IOError("Uncertainty option is not currently available for Fragility analysis. "
                              "Please change uncertainty to False or use Risk analysis option.")
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
    Creates a fault tree and computes minimal cut sets given a events with type and
    dependency.

    The minimal cut sets are computed using the MOCUS algorithm described in the document
    below.
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
        Helper for getting or creating Event objects
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

    Two approaches, 1 and 2, are used for top event risk calculation. When the
    input parameter, lite, is True only Approach 2 is performed. When lite is
    False, both Approach 1 and 2 are performed.

    Approach 1: This is a comprehensive calculation where basic event fragilities
    are first convolved with the hazard to calculate the risks. The basic event
    risks are then propagated through the fault tree as point estimates. This
    allows for the calculation of importance measures and all the importance
    measures are calculated and results are output into the corresponding folder.
    The top event risk is calculated using three methods: Rare event approximation
    Upper bound, and Min Max. Risk calculated using approach 1 is provided by
    the toprisk_1 function. The results calculation using approach 1 are saved
    in the approach_1 folder.

    Approach 2: In approach 2, the fragilities are propagated through the fault
    tree in each bin using the Min-Max method (exact calculation). This results
    in the top event fragility. The top event fragility is then convolved with the
    hazard to calculate risk. Risk calculated using approach 2 is provided by the
    toprisk_2 function. The results calculated using approach 2 are saved in the
    approach_2 folder.

    10 input parameters: 1,2,3,4,5,6,10,11 are for Fragility inputs and seismic PRA.
                         1,2,3,7,8,9,10,11 are for probabilistic risk inputs for uncertainty analysis.
                         1,2,3,10,11 are for deterministic risk inputs.

    (0) name[str]: A string identifier for the instance of this class.

    (1) logic[list]: A list with each row defining an event as follows:
                     'Event0', Event.AND|Event.OR, ['Dep0', 'Dep1', ...]

    (1) logic[filename]: A filename containing CSV data representing the events
                         in similar fashion as the list above.
                         Event0, AND|OR, Dep0, Dep1, ...

    (2) basic_events[list]: A list with each row defining a basic event
                            probability as follows: 'Basic_event0', 'PE', [Mean]

    (2) basic_events[filename]: A filename containing CSV data representing the
                                basic events in similar fashion as the list
                                above.
                                Basic_event0, PE, Mean

    (3) analysis[string]: Default is "Risk". The basic event prob. are assumed
                          to be risk values.

    (4) hazard[list]: A list with each row defining PGA and Prob. of Exceedance
                      as follows:
                      [[PGA_1, Prob. of exceedance_1],
                       [PGA_2, Prob. of exceedance_2],
                        ...
                       [PGA_n, Prob. of exceedance_n]]

              If None, the default list is [[0.0608, 0.01], [0.2124, 0.001],
              [0.4, 0.0001], [0.629, 1e-05], [0.9344, 1e-06], [1.3055, 1e-07]]

    (4) hazard[filename]: A filename containing CSV data representing the
                          hazard data in a similar fashion as the list above.
                          [[PGA, Prob. of exceedance], ...]

    (5) IM[list]: - If none is provided, a default range of [0.1,4] is assumed.
                  - If a list of two elements is provided, it is assumed that this
                  corresponds to the lower and upper bounds of the intensity
                  measures. This range is then split into nbins according to the
                  nbins input parameter.
                  - If a list longer than 2 elements is provided, these elements
                  are assumed to be the IM bin extents and the IM values for each
                  bin are calculated as the geometric mean of the extents of each
                  bin. The input parameter, nbins, is ignored in this case and the
                  number of bins is assumed to be equal to the length of this list
                  minus 1.

    (6) nbins[int]: Default is 15. Number of bins for Intensity Measure.
                    Can input any integer greater than 1.

    (7) uncertainty: Default is False (Point Estimate values).
                     If True, performs Monte Carlo simulation.

    (8) nsamp[int]: Default is 1. Number of samples for uncertainty analysis.
                    Can input any integer greater than 1.

    (9) seed[int]: Default is None (set seed from clock).
                   Can input any integer between 0 and (2**32 - 1).

    (10) lite[bool]: Default is False. When true, a 'lite' version of the
                     calculation will be performed, in which the top risk is
                     calculated only using Approach 2 (see above) and stored in
                     __top_risk_2. This approach is useful when importance
                     measures are not needed.

    (11) write_output[bool]: Default is false. If true, will write csv output
                             files. Approach 1 output files are written in the
                             approach_1 folder and approach_2 output files are
                             written in the approach_2 folder.
    """

    def __init__(self, name, logic, basic_events, analysis='default', hazard=None, IM=None,
                 nbins=15, uncertainty=False, nsamp=1, seed=None, lite=False, write_output=False):

        # name
        self.__name = name

        # create an object 'logic' of class 'FaultTree' by inputting the logic of the fault tree
        self.__logic = FaultTree(logic)

        # import minimal cut sets from object 'logic'
        self.__mcsets = self.__logic.mocus

        # read basic events file
        self.__bas_events = self.__readEventsFile(basic_events)

        # Type of Analysis
        self.__antype = analysis

        # Number of samples
        # TODO: throw error if nsamp is provided but uc is false
        self.__nsamp = self.__parValue(nsamp, 'nsamp')

        # Uncertainty Analysis
        self.__uc = uncertainty

        # set seed for the uncertainty analysis
        # TODO: throw error if seed is provided but uc is false
        self.__seed = self.__parValue(seed, 'seed')

        # If true, a 'lite' version of quantification will be performed as
        # described above
        self.__lite = lite

        # If true, outputs are written as csv files
        self.__write_output = write_output

        if self.__antype.lower() == "fragility":
            # read Hazard curve
            if hazard is None:
                self.__hazard = [[0.0608, 0.01], [0.2124, 0.001], [0.4, 0.0001],
                                 [0.629, 1e-05], [0.9344, 1e-06], [1.3055, 1e-07]]
            else:
                self.__hazard = self.__readHazardFile(hazard)

            # Intensity Measure calculations
            # Intensity measure is not provided, default is assumed
            if IM is None:
                self.__imrang = [0.1, 4]
            # When provided, check that it is a list
            if not isinstance(IM, list):
                raise TypeError("The supplied items of IM range must be a list.")
            # If 2 elements are provided, assume that it is a range and
            # calculate the IMs and IMextents according to the number of bins
            if len(IM) == 2:
                self.__imrang = IM
                # Number of bins for Intensity Measure
                self.__nbins = self.__parValue(nbins, 'nbins')
                # list of Intensity Measure bin values and the extents of the bins
                self.__im, self.__imextents = self.__bins(self.__imrang, self.__nbins)
            # If a list is provided assume that the list is the bin extents and
            # calculate bin IMs as the geometric means of the extents of each bin
            # Also nbins is calculated as the number of extents minus 1 and the
            # input nbins is ignored
            if len(IM) > 2:
                self.__imextents = IM
                self.__im = range(len(IM)-1) # initializing im
                self.__nbins = len(IM) - 1
                from numpy import sqrt
                for i in range(0, len(self.__im)):
                    self.__im[i] = sqrt(IM[i] * IM[i+1]) # geomean

            # Interpolating/Exterpolating hazard curve based on Intensity Measure bin values
            self.__haz_freq, self.__haz_freq_deltas = self.__hazInterp(self.__hazard, self.__im, self.__imextents)

            # dictionary of basic events (event objects) for fragility input
            self.__bnodes_frag = self.__BEprob(self.__bas_events, self.__antype, self.__logic.nodes,
                                               self.__im, self.__nsamp, self.__uc, self.__seed)

            # Top event fragility (lognormal parameters and plot)
            self.__topfragility, self.__ln = self.__TOPfragility(
                self.__Min_max, self.__mcsets, self.__MCprob,
                self.__lnparameters, self.__im)

            # Calculating top event risk using approach 2
            self.__bins_top_risk_2 = [x*y for x, y in zip(self.__topfragility, self.__haz_freq_deltas)]
            self.__bins_top_risk_2_mod = [x*y for x, y in zip(self.__topfragility, self.__haz_freq)]
            self.__top_risk_2 = [sum(self.__bins_top_risk_2)]

            # Writing the results from approach 2
            if self.__write_output:
                self.__approach_2_results(self.__name, self.__im, self.__haz_freq,
                                          self.__topfragility, self.__haz_freq_deltas,
                                          self.__bins_top_risk_2)

            if not (self.__lite):
                # dictionary of basic events risk (convolving fragility and hazard)
                # These values will be used in the importance measure calculations.
                self.__bnodes = self.__risk(self.__bnodes_frag, self.__haz_freq_deltas)

        elif self.__antype.lower() == "risk":
            # No hazard; values for basic events are assumed to be risk values,
            # and not fragility values
            self.__im = None

            # dictionary of basic events risk (risk inputs)
            self.__bnodes = self.__BEprob(self.__bas_events, self.__antype, self.__logic.nodes,
                                          self.__im, self.__nsamp, self.__uc, self.__seed)

        else:
            raise ValueError("The analysis type should be either `fragility` or `risk`.")

        if not self.__lite:
            # list of lists of minimal cutset probabilities
            self.__mc_prob = self.__MCprob(self.__mcsets)

            # top event risk using approach 1
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
            self.__write_output = write_output
            if self.__write_output:
                self.__approach_1_results(self.__name, self.__top_upper_bound,
                                          self.__mcsets, self.__mc_prob, self.__mc_im,
                                          self.__top_cal, self.__bas_events,
                                          self.__count, self.__bnodes, self.__be_im)

    @property
    def name(self):
        """Return the name attribute"""
        return self.__name

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
    def toprisk_1(self):
        """Return TOP event risk using approach 1"""
        if self.__lite:
            raise IOError("top_risk_1, which is the risk calculated from Approach 1 "
                          "is only available for a full analysis. Currently, the parameter "
                          "lite is set to True and only an Approach 2 analysis is performed.")
        else:
            return (self.__top_upper_bound[0], self.__top_rare[0], self.__top_exact[0])

    @property
    def toprisk_2_info(self):
        """Return the bin IM, bin hazard, bin fragility, bin hazard delta, and
           the top risk for each bin (haz delta * fragility) that was used to
           calculate the TOP event risk using approach 2"""
        return (self.__im, self.__haz_freq, self.__topfragility, self.__haz_freq_deltas, self.__bins_top_risk_2, self.__bins_top_risk_2_mod)

    @property
    def toprisk_2(self):
        """Return TOP event Risk using approach 2"""
        return self.__top_risk_2

    @property
    def be_im_ratio(self):
        """Return the basic event ratio importance measures"""
        return (self.__be_im['BE_fv'], self.__be_im['BE_rrr'], self.__be_im['BE_rir'])

    @property
    def be_im_diff(self):
        """Return the basic event difference importance measures"""
        return (self.__be_im['BE_rri'], self.__be_im['BE_rii'], self.__be_im['BE_bi'])

    def __str__(self):
        """
        Return the name of the class
        """
        return str(self.__name)

    def __repr__(self):
        """
        Display the name of the class
        """
        return self.__name

    @staticmethod
    def __BEprob(bas_events, antype, nodes, intmes, nsamp, uncert, seed):
        """
        Function assigns distribution type for Basic_event Nodes and calculates
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
                warnings.warn("Basic event is not used in logic of the fault tree: " + name)
                bnode = Event(name)
                bnode.dist = (dist, antype)
                # Probabilites of the nodes are assigned in the event class
                # according to the distribution and the analysis type
                bnode.prob = (probs, antype, intmes, uncert, nsamp, seed)
                bnodes[name] = bnode
        return bnodes

    ################## Minimal Cut Set Probabilites and Importance Measues ###################

    @staticmethod
    def __MCprob(cutsets):
        """
        Function calculates the probabilities of cutsets when a list of cutsets
        is provided. Result is a list of probabilities.
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
        Function calculates the probability of TOP event using min-max
        approach (exact)
        """
        import itertools
        sub = []
        # Evaluating the sets that occur in the union of the sets in
        # the variable, cutsets
        for l in range(1, len(cutsets)+1):
            for subset in itertools.combinations(cutsets, l):
                subset = set(list(itertools.chain(*subset)))
                sub.append(list(subset))
        # Calculating the probabilities associated with each of the sets
        sub_prob = mcprob(sub)
        # __Min_exact assigns the appropriate signs and sums all the
        # probabilities
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
            # get the index position of basic event in minimal cut sets
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

    ################## End Basic Event Importance Measures #####################

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
        imext = np.linspace(imrange[0], imrange[1], nbins+1)
        for i in range(0, len(imext)-1):
            im.append(0.5*(imext[i]+imext[i+1]))
        return im, imext

    @staticmethod
    def __lnparameters(intmes, fragility):
        """
        Function for getting the lognormal parameters - median and sd
        using least square fit
        """
        from scipy.optimize import curve_fit
        (logpara, logcov) = curve_fit(Quantification.__logcdf, intmes, fragility, maxfev=1000)
        return logpara, logcov


    @staticmethod
    def __logcdf(intmes, amedian, beta):
        """
        Function for fitting conditional probabilities into a lognormal cdf
        """
        from scipy.stats import lognorm
        return lognorm.cdf(intmes, beta, loc=0, scale=amedian)

    @staticmethod
    def __hazInterp(haz, im, imext):
        """
        Function for interpolating the hazard curve based on range of intensity measures
        """
        haz_im = zip(*haz)[0]
        haz_freq = zip(*haz)[1]

        from scipy import interpolate
        import numpy as np
        # Linear interpolation in the log-log scale interpretation of the
        # hazard curve. This is in accordance with ASCE 43, where the hazard
        # curve is assumed to be multi linear in the log-log scale
        yinterp = interpolate.interp1d(np.log10(haz_im), np.log10(haz_freq),
                                       fill_value='extrapolate')
        y_interp = 10**yinterp(np.log10(im))
        # Calculating hazard deltas that will be used in convolution
        y_interp_ext = 10**yinterp(np.log10(imext))
        deltas = []
        for i in range(0, len(y_interp_ext)-1):
            deltas.append(y_interp_ext[i] - y_interp_ext[i+1])

        # Plot Hazard Curve
        # fig, ax = plt.subplots()
        # ax.loglog(haz_IM,haz_freq)
        # ax.grid()
        # ax.loglog(im,Yinterp,'ro')
        # plt.show()
        return y_interp, deltas

    @staticmethod
    def __risk(bnodes, deltas):
        """
        Function for calculating basic event risk by convolving the fragility
        of the basic event with the hazard. The resultant risk is assigned to
        basic events as a probability. These risks are propagated
        through the fault tree (using one of the three approximations - Upper
        Bound, Rare Event or Min Max) and the top event risk is calculated.

        Convolution for each basic event is performed as:
        risk = sum[fragility(bin) * haz_delta(bin)]
        across all the bins. The delta for each bin is the width of the bin
        along the MAFE axis of the bin. Refer to MCEER report 08-0019 by Huang
        and Whittaker for a description of the convolution.
        """
        for key in bnodes:
            risk = [sum([x*y for x, y in zip(bnodes[key].prob, deltas)])]
            bnodes[key].dist = ("PE", "risk")
            bnodes[key].prob = (risk, "risk", None, False, 1, None)
        return bnodes

    @staticmethod
    def __parValue(value, name):
        """
        Function for raising input errors (nbins, nsamp, seed)
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
    def __readHazardFile(fname):
        """
        Function for reading the Hazard file.
        """
        if isinstance(fname, str):
            if not os.path.exists(fname):
                raise IOError("The filename {} does not exist.".format(fname))
            else:
                data = []
                with open(fname) as fid:
                    lines = fid.readlines()
                for line in lines:
                    items = line.split(',')
                    if len(items) > 2:
                        raise IOError("The hazard file {} should only have two items per row.".format(fname))
                    else:
                        data.append([float(items[0].strip()), float(items[1].strip())])
        elif isinstance(fname, list):
            data = fname
        else:
            raise TypeError(
                "The supplied input for hazard, {}, must be a filename or a list.".format(fname))
        return data

    @staticmethod
    def __readEventsFile(fname):
        """
        Function for reading the events file
        """
        if isinstance(fname, str):
            if not os.path.exists(fname):
                raise IOError("The filename {} does not exist.".format(fname))
            else:
                data = []
                with open(fname) as fid:
                    lines = fid.readlines()
                for line in lines:
                    items = line.split(',')
                    if len(items) > 2:
                        data.append([items[0].strip(),
                                     items[1].strip().upper(),
                                     [float(x.strip()) for x in items[2:]]])
                    else:
                        data.append([float(items[0].strip()), float(items[1].strip())])
        elif isinstance(fname, list):
            data = fname
        else:
            raise TypeError(
                "The supplied input for basic_events, {}, must be a filename or a list.".format(fname))
        return data

    @staticmethod
    def __approach_2_results(name, im, haz_freq, topfragility, haz_freq_deltas, bins_top_risk_2):
        """
        Function for writing approach 2 results in csv format
        """
        # Create a folder for results
        if not os.path.exists(name+'_results'):
            os.mkdir(name+'_results')
        if not os.path.exists(name+'_results/approach_2'):
            os.mkdir(name+'_results/approach_2')
        # Writing results into a csv file
        import csv
        dirname = name + '_results/approach_2/'
        with open(dirname+'results.csv', 'w') as f1:
            writer = csv.writer(f1, delimiter=',', lineterminator='\n',)
            writer.writerow(['bin#', 'im', 'mafe', 'top_failure_prob',
                              'delta_mafe', 'bin_risk'])
            for i, a in enumerate(im):
                writer.writerow([i+1, im[i], haz_freq[i], topfragility[i],
                                 haz_freq_deltas[i], bins_top_risk_2[i]])


    @staticmethod
    def __approach_1_results(name, top_upper_bound, mcsets, mc_prob, mc_im, top_cal, bas_events, count,
                  bnodes, be_im):
        """
        Function for writing approach 1 results in csv format.
        """
        # Create a folder for results
        if not os.path.exists(name+'_results'):
            os.mkdir(name+'_results')
        if not os.path.exists(name+'_results/approach_1'):
            os.mkdir(name+'_results/approach_1')

        # Writing results into separate csv files
        import csv
        dirname = name + '_results/approach_1/'
        with open(dirname+'cutsets.csv', 'w') as f1:
            writer = csv.writer(f1, delimiter=',', lineterminator='\n',)
            writer.writerows([['Cut Sets', 'Prob/Freq', 'IM (%)'],
                              ['Total', top_upper_bound[0], '100']])
            for i in range(0, len(mcsets)):
                writer.writerow([str(list(mcsets[i])), mc_prob[i][0],
                                 mc_im[i][0]])

        with open(dirname+'top_event.csv', 'w') as f2:
            writer = csv.writer(f2, delimiter=',', lineterminator='\n',)
            writer.writerows([['Quantification Method', 'Prob/Freq', 'Mean',
                               'Median', '5th', '95th', 'SD']])
            for i, keyt in enumerate(top_cal):
                row = [keyt] + [top_cal[keyt][0][k]
                                for k in range(0, len(top_cal[keyt][0]))]
                writer.writerow(row)

        with open(dirname+'importance_measures.csv', 'w') as f3:
            writer = csv.writer(f3, delimiter=',', lineterminator='\n',)
            writer.writerows([['Basic Event', 'Count', 'Prob', 'FV', 'RRR',
                               'RIR', 'RRI', 'RII', 'BI']])
            for i, keyb in enumerate(bas_events):
                row = [keyb[0]] + [count[i]] + \
                    [bnodes[keyb[0]].prob[0]] + [be_im[key][i][0] for key in be_im]
                writer.writerow(row)

        wname = ['fv', 'rrr', 'rir', 'rri', 'rii', 'bi']
        for i, key in enumerate(be_im):
            with open(dirname+wname[i]+'.csv', 'w') as f4:
                writer = csv.writer(f4, delimiter=',', lineterminator='\n',)
                writer.writerows([['Basic Event', 'PE', 'Mean', 'Median', '5th', '95th', 'SD']])
                for j, keyb in enumerate(bas_events):
                    row = [keyb[0]] + [be_im[key][j][k]
                                       for k in range(0, len(be_im[key][0]))]
                    writer.writerow(row)

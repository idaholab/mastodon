#!/usr/bin/env python
"""Provides FaultTree class for creating fault trees and minimum cut sets given a list of events."""
import os

class Event(object):
    """
    A basic node object for creating fault tree (see FaultTree)

    Inputs:
        name[str]: The name of the Event
        operator[Event.OR|Event.AND|None]: (optional) The node operator, use None for basic
                                                   event nodes.
    """
    # Define the node operators
    OR = 0
    AND = 1

    def __init__(self, name, operator=None):
        self.__name = name
        self.__children = []
        self.__operator = None
        self.operator = operator

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
    Creates a fault tree and computes minimum cut sets given a events with type and dependency.

    The minum cut sets are computed using the MOCUS algorithm as defined by:
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
                events = self.__readEventsFile(events)

        elif not isinstance(events, list):
            raise TypeError("The supplied 'events' must be a filename or a list.")

        self.__nodes, self.__root = self.__buildTree(events)
        self.__min_cut_sets = self.__computeMiniumCutSets()

    @property
    def root(self):
        """
        Return the root of the FaultTree
        """
        return self.__root

    @property
    def mocus(self):
        """
        Return the minimum cut sets as computed using the MOCUS algorithm.
        """
        return self.__min_cut_sets

    def __str__(self):
        """
        Print the tree.
        """
        return str(self.__root)

    def __computeMiniumCutSets(self):
        """
        Return the minimum cut sets for the given fault tree using the MOCUS method.
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

        # Minimum Cut Sets
        for p in out:
            out = FaultTree.__mocusRemoveSubset(p, out)

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
        Update the list of paths given a node for the minimum cut set calculation.
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
    def __mocusRemoveSubset(path, out):
        """
        Removes cut sets that contain other cut sets as a subset.
        """
        for o in out:
            if path.issubset(o):
                out.remove(o)
                break
        return out

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

#!/usr/bin/env python
"""Tests for the FTA.FaultTree object."""
import unittest
from mastodonutils.FTA import Event, FaultTree

class TestFaultTree(unittest.TestCase):
    """
    Test the FaultTree object.
    """

    @staticmethod
    def eventList():
        """
        Returns the test event list.
        """
        events = []
        events.append(['TOP', Event.AND, ['GATE1', 'GATE2']])
        events.append(['GATE1', Event.OR, ['FT-N/m-1', 'FT-N/m-2', 'FT-N/m-3']])
        events.append(['GATE2', Event.OR, ['B1', 'B3', 'B4']])
        events.append(['GATE3', Event.OR, ['B2', 'B4']])
        events.append(['FT-N/m-1', Event.AND, ['GATE3', 'B3', 'B5']])
        events.append(['FT-N/m-2', Event.AND, ['GATE3', 'B1']])
        events.append(['FT-N/m-3', Event.AND, ['B3', 'B5', 'B1']])
        return events

    def testFileTreeEventErrors(self):
        """
        Test error message for events input.
        """
        with self.assertRaises(IOError) as cm:
            FaultTree('not_a_valid_filename.txt')
        self.assertIn('not_a_valid_filename.txt', str(cm.exception))

        with self.assertRaises(TypeError) as cm:
            FaultTree(42)
        self.assertEqual("The supplied 'events' must be a filename or a list.", str(cm.exception))

    def testTreeFromList(self):
        """
        Test creating tree from list.
        """
        events = self.eventList()
        tree = FaultTree(events)
        self.assertTree(tree)
        self.assertMocus(tree.mocus)

    def testTreeFromFile(self):
        """
        Test creating tree from list.
        """
        tree = FaultTree('events.txt')
        self.assertTree(tree)
        self.assertMocus(tree.mocus)


    def assertTree(self, tree):
        """
        Helper to make sure the test tree is correct.
        """
        self.assertEqual(tree.root, 'TOP')
        self.assertEqual(tree.root.operator, Event.AND)

        # TOP
        node = tree.root
        self.assertEqual(node.children[0], 'GATE1')
        self.assertEqual(node.children[0].operator, Event.OR)
        self.assertEqual(node.children[1], 'GATE2')
        self.assertEqual(node.children[1].operator, Event.OR)

        # GATE1
        node = tree.root.children[0]
        self.assertEqual(node.children[0], 'FT-N/m-1')
        self.assertEqual(node.children[0].operator, Event.AND)
        self.assertEqual(node.children[1], 'FT-N/m-2')
        self.assertEqual(node.children[1].operator, Event.AND)
        self.assertEqual(node.children[2], 'FT-N/m-3')
        self.assertEqual(node.children[2].operator, Event.AND)

        # GATE2
        node = tree.root.children[1]
        self.assertEqual(node.children[0], 'B1')
        self.assertEqual(node.children[0].operator, None)
        self.assertEqual(node.children[1], 'B3')
        self.assertEqual(node.children[1].operator, None)
        self.assertEqual(node.children[2], 'B4')
        self.assertEqual(node.children[2].operator, None)

        # FT-N/m-1
        node = tree.root.children[0].children[0]
        self.assertEqual(node.children[0], 'GATE3')
        self.assertEqual(node.children[0].operator, Event.OR)
        self.assertEqual(node.children[1], 'B3')
        self.assertEqual(node.children[1].operator, None)
        self.assertEqual(node.children[2], 'B5')
        self.assertEqual(node.children[2].operator, None)

        # FT-N/m-2
        node = tree.root.children[0].children[1]
        self.assertEqual(node.children[0], 'GATE3')
        self.assertEqual(node.children[0].operator, Event.OR)
        self.assertEqual(node.children[1], 'B1')
        self.assertEqual(node.children[1].operator, None)

        # FT-N/m-3
        node = tree.root.children[0].children[2]
        self.assertEqual(node.children[0], 'B3')
        self.assertEqual(node.children[0].operator, None)
        self.assertEqual(node.children[1], 'B5')
        self.assertEqual(node.children[1].operator, None)
        self.assertEqual(node.children[2], 'B1')
        self.assertEqual(node.children[2].operator, None)

        # GATE3
        node = tree.root.children[0].children[0].children[0]
        self.assertEqual(node.children[0], 'B2')
        self.assertEqual(node.children[0].operator, None)
        self.assertEqual(node.children[1], 'B4')
        self.assertEqual(node.children[1].operator, None)

    def assertMocus(self, mocus):
        """
        Helper to make sure the mocus is correct.
        """
        gold = [{Event('B2'), Event('B1')},
                {Event('B4'), Event('B1')},
                {Event('B1'), Event('B5'), Event('B3')},
                {Event('B2'), Event('B5'), Event('B3')},
                {Event('B4'), Event('B5'), Event('B3')}]

        self.assertEqual(len(mocus), len(gold))
        for i in range(len(mocus)): #pylint: disable=consider-using-enumerate
            self.assertEqual(sorted([n.name for n in mocus[i]]), sorted([n.name for n in gold[i]]))

if __name__ == '__main__':
    unittest.main(module=__name__, verbosity=2)

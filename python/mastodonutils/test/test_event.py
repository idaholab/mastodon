#!/usr/bin/env python
"""Test for the FaultTree.Event object."""
import unittest
from mastodonutils.FaultTree import Event

class TestEvent(unittest.TestCase):
    """
    TestCase for Event object.
    """

    def testBasicEvent(self):
        """
        Basic node functions
        """
        node = Event('Test')
        self.assertEqual(node.name, 'Test')
        self.assertEqual(node.operator, None)

        with self.assertRaises(AttributeError) as cm:
            node.name = 'NewName'
        self.assertEqual("can't set attribute", str(cm.exception))

        self.assertEqual(node, 'Test')
        self.assertEqual(node, Event('Test'))

        with self.assertRaises(TypeError) as cm:
            node == 42 #pylint: disable=pointless-statement
        self.assertEqual("Comparison with between 'int' type is not valid.", str(cm.exception))

    def testChildren(self):
        """
        children attribute
        """
        node = Event('Test', Event.OR)
        node.children = [Event('C0'), Event('C1')]
        self.assertEqual(node.children[0], 'C0')
        self.assertEqual(node.children[1], 'C1')

        with self.assertRaises(TypeError) as cm:
            node.children = 42 #pylint: disable=redefined-variable-type
        self.assertEqual("The supplied items must be a list.", str(cm.exception))

        with self.assertRaises(TypeError) as cm:
            node.children = [Event('C0'), 1980]
        self.assertEqual("Supplied values must be Event objects.", str(cm.exception))

        with self.assertRaises(AttributeError) as cm:
            node.operator = None
        self.assertIn("The operator can not be set", str(cm.exception))

        node = Event('Test')
        with self.assertRaises(AttributeError) as cm:
            node.children = [Event('C0'), Event('C1')]
        self.assertIn("can not have children", str(cm.exception))

    def testOperator(self):
        """
        operator attribute
        """
        node = Event('Test', Event.OR)
        self.assertEqual(node.operator, Event.OR)

        with self.assertRaises(TypeError) as cm:
            node.operator = 42
        self.assertIn('The operator must be', str(cm.exception))

        node.operator = Event.AND
        self.assertEqual(node.operator, Event.AND)

        node.operator = None
        self.assertEqual(node.operator, None)

    def testString(self):
        """
        test print operations
        """
        node = Event('TOP', Event.OR)
        node.children = [Event('B0'), Event('B1')]
        string = str(node)
        self.assertIn('TOP', string)
        self.assertIn('  B0', string)
        self.assertIn('  B1', string)

        self.assertEqual(repr(node), 'TOP')


if __name__ == '__main__':
    unittest.main(module=__name__, verbosity=2)

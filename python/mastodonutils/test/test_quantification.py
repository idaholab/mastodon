#!/usr/bin/env python
"""Tests for the FTA.Quantification object."""
import unittest
from mastodonutils.FTA import Quantification


class TestQuantification(unittest.TestCase):
    """
    Test the Quantification object.
    """

    def testInputs(self):
        """
        Input values
        """
        # File Inputs
        with self.assertRaises(IOError) as cm:
            Quantification(logic='not_a_valid_filename.txt', bas_events='not_a_valid_filename.txt',
                           hazard='not_a_valid_filename.txt')
        self.assertIn('does not exist', str(cm.exception))

        with self.assertRaises(TypeError) as cm:
            Quantification(logic=42, bas_events=42, hazard=42)
        self.assertIn("must be a filename or a list.", str(cm.exception))

        # Inputs for Fragility
        inputs = Quantification('SSI_logic.txt', 'bas_events_ln.txt', analysis='Fragility',
                                hazard='hazard.txt', IM=[0.1, 4], nbins=15)
        self.assertEqual(inputs.logic, [['TE', 0, ['IE3', 'IE4']], ['IE4', 0, ['C4']],
                                        ['IE3', 0, ['C3', 'IE2']], ['IE2', 1, ['C2', 'IE1']],
                                        ['IE1', 0, ['C1']]])
        self.assertEqual(inputs.basic_events, [['C1', 'LNORM', [1.88, 0.5]],
                                               ['C2', 'LNORM', [3.78, 0.79]],
                                               ['C3', 'LNORM', [2.33, 0.76]],
                                               ['C4', 'LNORM', [3.66, 0.45]]])
        self.assertEqual(inputs.antype, 'Fragility')
        self.assertEqual(inputs.hazard, [[0.0608, 0.01], [0.2124, 0.001], [0.4, 0.0001],
                                         [0.629, 1e-05], [0.9344, 1e-06], [1.3055, 1e-07]])
        self.assertEqual(inputs.imrang, [0.1, 4])
        self.assertEqual(inputs.nbins, 15)

        with self.assertRaises(TypeError) as cm:
            Quantification('SSI_logic.txt', 'bas_events_ln.txt',
                           analysis='Fragility', hazard='hazard.txt', IM=42)
        self.assertEqual("The supplied items of IM range must be a list.", str(cm.exception))

        with self.assertRaises(TypeError) as cm:
            Quantification('SSI_logic.txt', 'bas_events_ln.txt',
                           analysis='Fragility', hazard='hazard.txt', nbins=[42])
        self.assertEqual("The supplied value of nbins must be an integer.", str(cm.exception))

        with self.assertRaises(ValueError) as cm:
            Quantification('SSI_logic.txt', 'bas_events_ln.txt',
                           analysis='Fragility', hazard='hazard.txt', nbins=-15)
        self.assertEqual("The supplied value of nbins must be a +ve integer.",
                         str(cm.exception))

        # Inputs for Risk
        inputs = Quantification('events.txt', 'bas_events.txt', analysis='Risk')
        self.assertEqual(inputs.logic, [['TOP', 1, ['GATE1', 'GATE2']],
                                        ['GATE1', 0, ['FT-N/m-1', 'FT-N/m-2', 'FT-N/m-3']],
                                        ['GATE2', 0, ['B1', 'B3', 'B4']],
                                        ['GATE3', 0, ['B2', 'B4']],
                                        ['FT-N/m-1', 1, ['GATE3', 'B3', 'B5']],
                                        ['FT-N/m-2', 1, ['GATE3', 'B1']],
                                        ['FT-N/m-3', 1, ['B3', 'B5', 'B1']]])
        self.assertEqual(inputs.basic_events, [['B1', 'PE', [0.01]], ['B2', 'PE', [0.02]],
                                               ['B3', 'PE', [0.03]], ['B4', 'PE', [0.04]],
                                               ['B5', 'PE', [0.05]]])
        self.assertNotEqual(inputs.antype, 'Fragility')
        self.assertEqual(inputs.uncertainty, False)
        self.assertEqual(inputs.nsamp, 1)
        self.assertEqual(inputs.seed, None)

        inputs = Quantification('events.txt', 'bas_events.txt', analysis='Risk',
                                uncertainty=True, nsamp=1000, seed=436546754)
        self.assertEqual(inputs.uncertainty, True)
        self.assertEqual(inputs.nsamp, 1000)
        self.assertEqual(inputs.seed, 436546754)

        with self.assertRaises(TypeError) as cm:
            Quantification('events.txt', 'bas_events.txt', analysis='Risk',
                           uncertainty=True, nsamp=[1], seed=42.1)
        self.assertEqual("The supplied value of nsamp must be an integer.", str(cm.exception))

        with self.assertRaises(TypeError) as cm:
            Quantification('events.txt', 'bas_events.txt', analysis='Risk',
                           uncertainty=True, nsamp=10, seed=42.0)
        self.assertEqual("The supplied value of seed must be an integer.", str(cm.exception))

        with self.assertRaises(ValueError) as cm:
            Quantification('events.txt', 'bas_events.txt', analysis='Risk',
                           uncertainty=True, nsamp=-10, seed=42.0)
        self.assertEqual("The supplied value of nsamp must be a +ve integer.",
                         str(cm.exception))

        with self.assertRaises(ValueError) as cm:
            Quantification('events.txt', 'bas_events.txt', analysis='Risk',
                           uncertainty=True, nsamp=10, seed=-42)
        self.assertEqual("The supplied value of seed must be a +ve integer.",
                         str(cm.exception))

    def testTOPrisk(self):
        """
        Function for asserting FTA top event risk.
        """
        fta = Quantification('events.txt', 'bas_events_norm.txt', analysis='Risk')
        self.assertRisk(fta)
        self.assertIMratio(fta.be_im_ratio)
        self.assertIMdiff(fta.be_im_diff)

    def assertRisk(self, fta):
        """
        Helper to make sure the FTA top event risk is correct.
        """
        # upper bound
        self.assertEqual(fta.toprisk[0], 0.000704853860316601)
        # rare event
        self.assertEqual(fta.toprisk[1], 0.0007050000000000001)
        # min-max
        self.assertEqual(fta.toprisk[2], 0.0006940240000000001)

    def assertIMratio(self, imratio):
        """
        Helper to make sure basic event ratio importance measures are correct.
        """
        # Fussell-Vesely for B1, B2, B3, B4, B5
        self.assertEqual(imratio[0][0][0], 0.8723950251527652)
        self.assertEqual(imratio[0][1][0], 0.3263002630028771)
        self.assertEqual(imratio[0][2][0], 0.14896258066840812)
        self.assertEqual(imratio[0][3][0], 0.6525835012002473)
        self.assertEqual(imratio[0][4][0], 0.14896258066840812)

        # Risk Reduction Ratio for B1, B2, B3, B4, B5
        self.assertEqual(imratio[1][0][0], 7.831866196407231)
        self.assertEqual(imratio[1][1][0], 1.4839994007665414)
        self.assertEqual(imratio[1][2][0], 1.174913088939574)
        self.assertEqual(imratio[1][3][0], 2.87706550337578)
        self.assertEqual(imratio[1][4][0], 1.174913088939574)

        # Risk Reduction Ratio for B1, B2, B3, B4, B5
        self.assertEqual(imratio[2][0][0], 86.11110290839181)
        self.assertEqual(imratio[2][1][0], 16.960273261286435)
        self.assertEqual(imratio[2][2][0], 5.808754991114076)
        self.assertEqual(imratio[2][3][0], 16.637742224118053)
        self.assertEqual(imratio[2][4][0], 3.826894185637253)

    def assertIMdiff(self, imdiff):
        """
        Helper to make sure basic event ratio importance measures are correct.
        """
        # Risk Reduction Difference for B1, B2, B3, B4, B5
        self.assertEqual(imdiff[0][0][0], 0.0006148556603167155)
        self.assertEqual(imdiff[0][1][0], 0.0002298847599567777)
        self.assertEqual(imdiff[0][2][0], 0.000104933860316625)
        self.assertEqual(imdiff[0][3][0], 0.000459863310226738)
        self.assertEqual(imdiff[0][4][0], 0.000104933860316625)

        # Risk Increase Difference for B1, B2, B3, B4, B5
        self.assertEqual(imdiff[1][0][0], 0.05999088944078346)
        self.assertEqual(imdiff[1][1][0], 0.011249660219925572)
        self.assertEqual(imdiff[1][2][0], 0.0033894695188034785)
        self.assertEqual(imdiff[1][3][0], 0.01102232297330552)
        self.assertEqual(imdiff[1][4][0], 0.001992547279452972)

        # Birnbaum Difference for B1, B2, B3, B4, B5
        self.assertEqual(imdiff[2][0][0], 0.06060574510110017)
        self.assertEqual(imdiff[2][1][0], 0.01147954497988235)
        self.assertEqual(imdiff[2][2][0], 0.0034944033791201035)
        self.assertEqual(imdiff[2][3][0], 0.011482186283532259)
        self.assertEqual(imdiff[2][4][0], 0.002097481139769597)


if __name__ == '__main__':
    unittest.main(module=__name__, verbosity=2)

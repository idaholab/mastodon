import numpy as np
from scipy.stats import gamma
import sys
np.set_printoptions(threshold=sys.maxsize)

def RJB2REPI(M, R):
    """
    Converts Rjb to Repi
    Inputs:
      M - Moment Magnitude
      Repi - Epicentral Distance

    Outputs:
      Rjb - Joyner-Boore Distance
    """
    # Initialize variables
    d, mean, sigma, beta, alfa, median = R, 0, 0, 0, 0, 0

    if M >= 3.25 and M <= 6.75:

        # Compute average
        mean = (25.2859086858311 - 24.7579132762745 * M + 9.1067957463996 * M ** 2 - 1.4924537794753 * M ** 3 + 0.0943269381529 * M ** 4) + \
               d * (1.0258015468051 - 0.9546610173338 * M + 0.3306850146342 * M ** 2 - 0.0507022784386 * M ** 3 + 0.0029166620192 * M ** 4) + \
               d ** 2 * (-0.0087847623757 + 0.008078532233 * M - 0.0027616254404 * M ** 2 + 0.0004175139082 * M ** 3 - 0.0000236750349 * M ** 4) + \
               d ** 3 * (0.0000210744968 - 0.0000192014444 * M + 0.0000064976695 * M ** 2 - 0.0000009719658 * M ** 3 + 0.0000000545396 * M ** 4)

        # Compute sigma
        sigma = (50.4650608321051 - 48.0676507162484 * M + 17.1052739585892 * M ** 2 - 2.7022941754681 * M ** 3 + 0.1613658945296 * M ** 4) + \
                d * (0.7371820139324 - 0.6752613553 * M + 0.2293488841656 * M ** 2 - 0.0343077682523 * M ** 3 + 0.00191302107 * M ** 4) + \
                d ** 2 * (-0.0056666537501 + 0.0051882394919 * M - 0.0017625955284 * M ** 2 + 0.0002639958786 * M ** 3 - 0.0000147580962 * M ** 4) + \
                d ** 3 * (0.0000113403782 - 0.000010388538 * M + 0.0000035364421 * M ** 2 - 0.0000005318502 * M ** 3 + 0.0000000299318 * M ** 4)

        beta = (sigma ** 2) / mean;  # scale parameter
        alfa = (mean / sigma) ** 2;  # shape parameter
        median = gamma.ppf(0.5, alfa, beta)

    elif M > 6.75 and M <= 8.25:

        # Compute average
        mean = (41330.5714998843 - 22733.3089137796 * M + 4692.74801622372 * M ** 2 - 431.089171902869 * M ** 3 + 14.8828551078011 * M ** 4) + \
               d * (-2416.16571885248 + 1313.68542796199 * M - 267.398312525097 * M ** 2 + 24.1469682260178 * M ** 3 - 0.8160573655126 * M ** 4) + \
               d ** 2 * (20.7418579312278 - 11.2634077165846 * M + 2.2898378394305 * M ** 2 - 0.2065409463611 * M ** 3 + 0.0069730227999 * M ** 4) + \
               d ** 3 * (-0.0518782359661 + 0.0281154133727 * M - 0.0057044823582 * M ** 2 + 0.00051353589 * M ** 3 - 0.0000173053232 * M ** 4)

        # Compute sigma
        sigma = (12207.5188429977 - 7001.4407532747 * M + 1515.67332601025 * M ** 2 - 146.912852179082 * M ** 3 + 5.388523704157 * M ** 4) + \
                d * (-1318.61674483966 + 718.285967089764 * M - 146.541924947989 * M ** 2 + 13.2688348378962 * M ** 3 - 0.4497899372784 * M ** 4) + \
                d ** 2 * (14.9495278339686 - 8.1301182336872 * M + 1.6558268525748 * M ** 2 - 0.1496655480247 * M ** 3 + 0.0050646642932 * M ** 4) + \
                d ** 3 * (-0.041450318097 + 0.0224827452608 * M - 0.004566514641 * M ** 2 + 0.0004116109653 * M ** 3 - 0.0000138901781 * M ** 4)

        beta = (sigma ** 2) / mean;  # scale parameter
        alfa = (mean / sigma) ** 2;  # shape parameter
        median = gamma.ppf(0.5, alfa, beta)

    Repi = d + median

    return Repi


def REPI2RJB(M, Repi):
    """
    Converts Repi to Rjb
    Inputs:
      M - Moment Magnitude
      Repi - Epicentral Distance

    Outputs:
      Rjb - Joyner-Boore Distance
    """

    # Initialize variables
    d, mean, sigma, beta, alfa, median = 0, 0, 0, 0, 0, 0

    if M >= 3.25 and M <= 6.75:

        while Repi - d - median > 0.001:
            d += 0.01

            # Compute Average
            mean = (25.2859086858311 - 24.7579132762745 * M + 9.1067957463996 * M ** 2 - 1.4924537794753 * M ** 3 + 0.0943269381529 * M ** 4) + \
                   d * (1.0258015468051 - 0.9546610173338 * M + 0.3306850146342 * M ** 2 - 0.0507022784386 * M ** 3 + 0.0029166620192 * M ** 4) + \
                   d ** 2 * (-0.0087847623757 + 0.008078532233 * M - 0.0027616254404 * M ** 2 + 0.0004175139082 * M ** 3 - 0.0000236750349 * M ** 4) + \
                   d ** 3 * (0.0000210744968 - 0.0000192014444 * M + 0.0000064976695 * M ** 2 - 0.0000009719658 * M ** 3 + 0.0000000545396 * M ** 4)

            # Compute Sigma
            sigma = (50.4650608321051 - 48.0676507162484 * M + 17.1052739585892 * M ** 2 - 2.7022941754681 * M ** 3 + 0.1613658945296 * M ** 4) + \
                    d * (0.7371820139324 - 0.6752613553 * M + 0.2293488841656 * M ** 2 - 0.0343077682523 * M ** 3 + 0.00191302107 * M ** 4) + \
                    d ** 2 * (-0.0056666537501 + 0.0051882394919 * M - 0.0017625955284 * M ** 2 + 0.0002639958786 * M ** 3 - 0.0000147580962 * M ** 4) + \
                    d ** 3 * (0.0000113403782 - 0.000010388538 * M + 0.0000035364421 * M ** 2 - 0.0000005318502 * M ** 3 + 0.0000000299318 * M ** 4)

            median = gamma.ppf(0.5, alfa, beta)
    elif M > 6.75 and M <= 8.25:

        while Repi - d - median > 0.001:
            d += 0.01

            # Compute average
            mean = (41330.5714998843 - 22733.3089137796 * M + 4692.74801622372 * M ** 2 - 431.089171902869 * M ** 3 + 14.8828551078011 * M ** 4) + \
                   d * (-2416.16571885248 + 1313.68542796199 * M - 267.398312525097 * M ** 2 + 24.1469682260178 * M ** 3 - 0.8160573655126 * M ** 4) + \
                   d ** 2 * (20.7418579312278 - 11.2634077165846 * M + 2.2898378394305 * M ** 2 - 0.2065409463611 * M ** 3 + 0.0069730227999 * M ** 4) + \
                   d ** 3 * (-0.0518782359661 + 0.0281154133727 * M - 0.0057044823582 * M ** 2 + 0.00051353589 * M ** 3 - 0.0000173053232 * M ** 4)

            # Compute sigma
            sigma = (12207.5188429977 - 7001.4407532747 * M + 1515.67332601025 * M ** 2 - 146.912852179082 * M ** 3 + 5.388523704157 * M ** 4) + \
                    d * (-1318.61674483966 + 718.285967089764 * M - 146.541924947989 * M ** 2 + 13.2688348378962 * M ** 3 - 0.4497899372784 * M ** 4) + \
                    d ** 2 * (14.9495278339686 - 8.1301182336872 * M + 1.6558268525748 * M ** 2 - 0.1496655480247 * M ** 3 + 0.0050646642932 * M ** 4) + \
                    d ** 3 * (-0.041450318097 + 0.0224827452608 * M - 0.004566514641 * M ** 2 + 0.0004116109653 * M ** 3 - 0.0000138901781 * M ** 4)

            beta = (sigma ** 2) / mean  # scale parameter
            alfa = (mean / sigma) ** 2  # shape parameter
            median = gamma.ppf(0.5, alfa, beta)

    Rjb = d

    return Rjb


def GroundMotionSim(M, Dist, Rval, Vs30, F, n):
    """
    This function takes geophysical inputs for earthquakes and generates realistic acceleration time-histories
    This is a python implementation of a previously developed method by (Sabetta, Pugliese, Fiorentino et al 2021)
    Source : https://link.springer.com/article/10.1007/s10518-021-01077-1

    ## Inputs ###
    M - Moment Magnitude
    Dist - String indicating which distance will be provided
          'Repi' - Epicentral Distance
          'Rjb' - Joyner-Boore Distance
    Rval - Distance value
    Vs30 - Shear Wave Velocity at a depth of 30m
    F - Fault Type
          'SS' - Strike-Slip
          'TF' - Thrust Fault
          'NF' - Normal Fault
    n - random seed

    ### Outputs ###
    # t - vector of time steps in (s)
    # Acc - Acceleration history in (cm/s^2)
    """

    M,Rval,Vs30,n = float(M),float(Rval),float(Vs30),int(n)

    # set seed for consistent acceleration histories
    np.random.seed(n)

    # Distance Conversion between Epicentral and Joyner-Boorse distances
    if Dist == 'Repi':
        Repi = Rval
        Rjb = REPI2RJB(M, Repi)
        R = Rjb
    elif Dist == 'Rjb':
        Rjb = Rval
        Repi = RJB2REPI(M, Rjb)
        R = Rjb

    ########################################
    ### Evaluate Arias Intensity and DV ###
    #######################################

    # Mixed-Effects Coefficients calibrated for Italian Earthquakes from ITA18 database
    a = np.array([-2.2907, 0.434])
    b1 = 0.249
    b2 = 0.495
    b11 = 1.4033
    b21 = -0.0881
    c1 = np.array([0.487, -0.098])
    c2 = np.array([-1.0667, 0.258])
    c3 = np.array([-0.0054, 0.002])
    k = np.array([-1.0309, -0.252])
    f1 = np.array([0.1185, -0.05])
    f2 = np.array([-0.0176, -0.032])

    Mref = np.array([7.5, 7])  # Reference Magnitudes
    h = np.array([5, 6])  # pseudo-depth
    tau = np.array([0.259, 0.078])  # between-event residual sd
    fis2s = np.array([0.393, 0.133])  # site-to-site residual sd
    fi0 = np.array([0.328, 0.145])  # site corrected residual sd
    sigma = np.array([0.574, 0.211])  # total standard deviation
    # sigma = np.sqrt(tau**2 + fis2s**2 + fi0**2) # total standard deviation
    Mh = 7  # Hinge Magnitude

    # Source Term FM for Arias Intensity (Equation (18))
    FM_AI = b11 * M + b21 * M ** 2

    # Source Term FM for Duration (Equation (12))
    if M <= Mh:
        FM_DV = b1 * (M - Mh)
    elif M > Mh:
        FM_DV = b2 * (M - Mh)

    # Combined Source Term
    FM = np.array([FM_AI, FM_DV])

    # Style of Faulting Term
    if F == 'SS':
        SF = [1, 0, 0]
        FSOF = f1
    elif F == 'TF':
        SF = [0, 1, 0]
        FSOF = f2
    elif F == 'NF':
        SF = [0, 0, 1]
        FSOF = 0

    # Distance Term FD (Equation (13))
    FD = (c1 * (M - Mref) + c2) * np.log10(np.sqrt(R ** 2 + h ** 2)) + c3 * np.sqrt(R ** 2 + h * 2)

    # Site-Effect Term (Equation (14))
    FS = a + k * np.log10(Vs30 / 800)

    # Combined Functional Form (Equation (11))
    sum = FM + FSOF + FD + FS
    PAR = 10 ** sum

    # Arias Intensity
    AI = PAR[0]
    AI *= 2 * 981 / np.pi

    # Significant Duration
    DV = PAR[1]

    ################################################################
    ### Central Frequency (Fc(t)) and Frequency Bandwidth (Fb(t) ###
    ################################################################

    # Parameters to Evaluate Central Frequency Fc(t)
    h = 10
    Rhyp = np.sqrt(Repi ** 2 + h ** 2)  # Focal Distance
    Tsp = Rhyp / 7  # Time delay between arrival of P and S waves (Ts - Tp)
    Ts = Tsp + DV / 2  # Time at which S-waves arrive
    Tp = Ts / 3  # Time at which P-waves arrive
    Tcoda = Tsp + 1.5 * DV  # Time at which coda waves arrive
    Tcc = Ts + 2 * DV
    sigmap = -(np.log(Tsp / Ts)) / 3  # sd of t for P pulse
    sigmas = (np.log(Tcc / Ts)) / 3  # sd of t for S pulse
    mup = np.log(Tp) + sigmap ** 2  # mean of t for P pulse (controls time at which max amplitude of P pulse is reached)
    mus = np.log(Ts) + sigmas ** 2  # mean of t for S pulse (controls time at which max amplitude of S pulse is reached)
    tcost = Ts + DV
    Ttot = 1.3 * (Tsp + 3 * DV)  # Total duration
    om_0 = 2 * np.pi / Ttot  # fundamental frequency
    tfc = Tp

    # Time and Frequency Vectors
    srate = 0.005
    nyq = 1 / (2 * srate)
    fzero = 1 / Ttot  # fundamental frequency
    fsamp = 2 * ((nyq - fzero) / (nyq / fzero))

    f = np.arange(fzero, nyq, fsamp)  # frequency vector
    t = np.arange(srate, Ttot, srate)  # time vector
    nt = len(t)

    # Fc dependence on distance, curve shift at Tp and Tcoda
    tval = t.copy()
    tmod = np.zeros((len(t), 1))

    for i in range(nt):
        if tval[i] < Tp:
            tmod[i, 0] = Tp
        else:
            tmod[i, 0] = tval[i]

    # Parameters for Fc(t)
    fnot = 3.5
    ft = 0.224
    fm = 0.208
    fs = 0.42

    # Calculate Fc(t) (Equation (19))
    Fc_t = np.zeros((len(t), 1))
    for j in range(nt):
        if tmod[j, 0] < tcost:
            Fc_t[j, 0] = np.exp(fnot - ft * np.log(tmod[j]) - fm * M + fs * np.log(Vs30 / 800))
        elif tmod[j, 0] >= tcost:
            Fc_t[j, 0] = np.exp(fnot - ft * np.log(tcost) - fm * M + fs * np.log(Vs30 / 800))

    # Fb/Fc Ratio
    Fb_Fc = 0.44 + 0.07 * M - 0.1 * np.log(Vs30 / 800)
    Fb_t = Fc_t * Fb_Fc  # Frequency Bandwidth

    #########################################
    ### Instantaneous Average Power Pa(t) ###
    #########################################
    # Represents the time envelope of the simulated accelerograms

    # Determine when Tcoda occurs
    id_coda = max(np.where(t <= Tcoda)[0])

    # Central Frequency at Tcoda
    fq = Fc_t[id_coda]

    # Parameters (Bind and Kotha 2020)
    Qc = 250 * (fq ** 0.29)  # coda Q-value
    PaP = np.zeros((len(t)))
    precoda = np.zeros((len(t)))

    for i in range(len(t)):
        # P-wave Contribution to Pa(t) in Equation (21)
        PaP[i] = (1 / t[i]) * (1 / np.sqrt(2 * np.pi) * sigmap) * np.exp(
            (-1 / (2 * sigmap ** 2)) * (np.log(t[i]) - mup) ** 2)

        # S-wave Contribution to Pa(t) in Equation (21)
        precoda[i] = (1 / t[i]) * (1 / np.sqrt(2 * np.pi) * sigmas) * np.exp(
            (-1 / (2 * sigmas ** 2)) * (np.log(t[i]) - mus) ** 2)

    # Accounting for Arias Intensity in Equation (21)
    inteP = np.trapz(PaP, t)
    inteS = np.trapz(precoda, t)
    AI_fact_P = AI / inteP
    AI_fact_S = AI / inteS
    Pa_P = PaP * AI_fact_P
    Pa_S = precoda * AI_fact_S

    # Coefficients in Equation (21)
    Pa_P *= (1 / 25) * AI
    Pa_S *= (24 / 25) * AI

    # S and P Wave contribution to Pa(t)
    Pa_t = Pa_P + Pa_S

    # Coda Wave Contribution to Pa(t)
    alpha = 2  # body wave parameter
    t1 = t[(id_coda + 1):]
    first = t1 ** (-alpha)
    second = np.exp((-1 * (2 * np.pi * fq * t1) / Qc))
    Azero = Pa_t[id_coda] / (first[0] * second[0])  # Source term representing effects of both S and P waves
    e = (first * second) * Azero

    # Combine S, P, and Coda Wave contributions to Pa(t) equation
    Pa_t = np.append(Pa_t[:id_coda + 1], e, axis=0)

    # Accounting for time-dependence
    inte = np.trapz(Pa_t, t)
    AI_fact = AI / inte
    Pa_t *= AI_fact

    ##############################
    ### Compute Brune Spectrum ###
    ##############################

    # Compute Brune Spectrum (Equations (8) and (9))
    beta = 3.2  # Shear wave velocity in km/s
    deltas = 40  # Stress drop in bars
    fcorn = 10 ** (1.341 + np.log10(beta * ((deltas) ** (1 / 3))) - 0.5 * M)  # corner frequency
    k_br = 0.03  # Filter for attenuaton operator (change to 0.04 for CA?)
    sorg = np.zeros((len(f), len(t)))
    sorg = ((2 * np.pi * f) ** 2) / (1 + (f / fcorn) ** 2)  # S(f)

    # Attenuation Operator (Boore 2003 Eq. 20)
    # Accounts for path-independent loss of high frequency in ground motions
    # possible missing -1
    D = np.exp(np.pi * k_br * f)
    shape = (sorg / D)

    # Compute input parameters to lognormal funciton Xs(t,f) (Equation (7))
    # Derived from Fc(t) and Fb(t)
    delta = np.sqrt(np.log(1 + ((Fb_t ** 2) / (Fc_t ** 2))))
    ln_beta_t = np.log(2 * np.pi * Fc_t) - ((delta ** 2) / 2)

    # Initialize variables
    PS_t_f = np.zeros((len(t), len(f)))
    LOGN = np.zeros((len(t), len(f)))
    MAXLOG = np.zeros((len(t)))
    id = np.zeros((len(t)), dtype=int)
    mediaLOG = np.zeros((len(t), len(f)))
    MAXBRUNE = np.zeros((len(t)))

    Brune = np.zeros((len(t), len(f)))
    for kz in range(len(t)):
        Brune[kz][:] = np.copy(shape)

    #######################
    ### Compute Xs(t,f) ###
    #######################

    # Compute lognormal function for Xs(t,f) Equation (6)
    # possible error, why use j instead of f(j) and where does om_0 come in?
    XS_t_f = np.zeros((len(t), len(f)))

    jidx = np.array(range(1, len(f) + 1))
    iidx = np.array(range(len(t)))

    LOGN = 1 / (jidx * np.sqrt(2 * np.pi) * delta) * np.exp(
        -1 * ((np.log(jidx) + np.log(om_0) - ln_beta_t) ** 2) / (2 * delta ** 2))

    for jz in range(len(t)):
        MAXLOG[jz] = max(LOGN[jz][:])
        currentid = np.where(LOGN[jz][:] == MAXLOG[jz])
        currentid = int(currentid[0][0])
        id[jz] = np.copy(int(currentid))

        MAXBRUNE[jz] = Brune[jz][id[jz]]
        Brune[jz][:] *= MAXLOG[jz] / MAXBRUNE[jz]

    # Modified Xs(t,f) Equation (10)
    for i in range(len(t)):
        for j in range(len(f)):
            if j < id[i]:
                mediaLOG[i][j] = np.sqrt(Brune[i][j] * LOGN[i][j])
                # Arithmetic Mean
                # mediaLOG[i][j] = (Brune[i][j] + LOGN[i][j])/2
            elif j >= id[i]:
                mediaLOG[i][j] = LOGN[i][j]

    XS_t_f = (Pa_t*mediaLOG.T).T

    ############################
    ### Generate Time Series ###
    ############################

    # Generate Time series of ground motion
    # Loop to create ACC (matrix with n ground motions)
    ACC = np.zeros((len(t)))
    aa = 0
    b = 2 * np.pi

    # Calculate Acceleration Time Series (Equations (22) and (23))
    tt = t

    # Generate Random Phases
    r = (b - aa) * np.random.rand(len(f), 1) + aa
    for kk in range(len(f)):
        # Equation (23)
        Cn = np.sqrt(2 * XS_t_f[:, kk])

        # Equation (22) cosine portion
        cosin = np.cos((kk * om_0 * tt) + r[kk])

        # Accleration Equation (22)
        a_t = Cn * cosin
        ACC += a_t


    # Convert Acc units to be in (g)
    ACC /= 981

    print('t=',t)
    print('Acc=',ACC.T.flatten())

    return t,ACC


if __name__ == '__main__':
    globals()[sys.argv[1]](*sys.argv[2:])

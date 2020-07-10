# A test of the BaselineCorrection function applied to an earthquake
# accelelogram. The data was obtained from records of the "Chi-Chi 1999-09-20
# 17:47:16 UTC" event available at the Strong Motion Virtual Data Center (VDC)
# @ strongmotioncenter.org
#
# The header included in the file obtained from the VDC, `vdc.chichi.A0126300_N`,
# was as follows:
#
# --------------------------------------------------------------------------------
# Uncorrected Acceleration  (Format v01.00 with 13 text lines) Orig: A0126300.ASC
# ChiChi,Taiwan Earthquake                September 20, 1999 17:47:15.670 UTC
# Hypocenter:23.860300 120.799500 10.33km(CWB)   Mw=7.6(CWB)
# Origin: 09/20/1999 17:47:15.670 UTC(CWB)Owner: CWB
# Station: TAP007 CWB 25.0758   121.5083  P007
# Site Geology: S_E (UBC 1997)
# Recorder: ETNA S/N A01    ( 3 Chns of  3 at Sta) Sensor:  S/N
# Rcrd Start Time:09/20/99 16:15:28.000 UTC(Q=3) Recordid:
# Sta Cha: 1     0 deg (Rcrdr Chan  1) Location:Free-field
# Raw record length: 134.000 sec  Uncor max= -71.6cm/s/s, at 42.615  sec
# Rec, filtered below:       Hz (periods over      secs), & above:       Hz
# Max:    -71.640 cm/s/s , at 42.615  sec
# Values used in headers to denote parameter is unknown/unspecified: -999, -999.0
# 100 Integer-header values follow on 10 lines, Format  (10I8)
#        1       1       3     100       1    -999    -999    -999    -999    -999
#     -999    -999    -999    -999    -999    -999    -999    -999    -999    -999
#     -999       1       3    -999     107    -999       0       3       3       3
#     -999    -999    -999    -999    -999    -999    1999     263       9      20
#       17      47      15      67       3    -999    -999    -999    -999       1
#     -999    -999       0       0    -999    -999    -999    -999    -999    -999
#     -999    -999    -999    -999    -999    -999    -999    -999    -999    -999
#     -999    -999    -999    -999    -999    -999    -999    -999    -999    -999
#     -999    -999    -999    -999    -999    -999    -999    -999    -999    -999
#     -999    -999    -999    -999    -999    -999    -999    -999    -999    -999
# 100 Real-header values follow on 17 lines, Format (6F13.6)
#     25.075800   121.508301     5.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000    23.860300   120.799500    10.330000
#      7.600000  -999.000000  -999.000000  -999.000000   152.600000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000     0.005000
#      0.000000     0.000000  -999.000000  -999.000000  -999.000000  -999.000000
#    134.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000     2.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000   -71.639852    42.615000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000
#   7  comment line(s) follow, each starting with a '|':
# | Station Map:http://scman.cwb.gov.tw/eqv3/accsta-e/acc-sta-e.htm
# | Original file, A0126300.ASC, created: January 08, 2001 17:59 UTC
# | http://www.cwb.gov.tw/V3.0e/index-e.htm
# | Converted to COSMOS format: July 18, 2001 22:35 UTC
# | http://www.cosmos-eq.org/cosmos_format_01.pdf
# | Retrieved from:
# | ftp://smdb.crustal.ucsb.edu/chichi/A0126300_N
# 26800  acceleration pts a  200 pts/sec, Units=cm/s/s     , Format=(1F14.9  )
# --------------------------------------------------------------------------------
#
# The remainder of the file was the acceleration data which has been copied
# to `chichi_A0126300_N.csv`

[Mesh]
  type = GeneratedMesh
  dim = 1
[]

[Problem]
  solve = false
[]

[Functions]
  [corrected_accel_func]
    type = BaselineCorrection
    data_file = chichi_A0126300_N.csv
    gamma = 0.5
    beta = 0.25
    accel_fit_order = 4
  []
[]

[Executioner]
  type = Transient
  start_time = 5e-03
  end_time = 134
  dt = 5e-03
[]

[Postprocessors]
  [check_func_value]
    type = FunctionValuePostprocessor
    function = corrected_accel_func
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Outputs]
  csv = true
[]

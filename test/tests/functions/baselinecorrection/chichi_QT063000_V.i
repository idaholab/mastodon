# A test of the BaselineCorrection function applied to an earthquake
# accelelogram. The data was obtained from records of the "Chi-Chi 1999-09-20
# 17:47:16 UTC" event available at the Strong Motion Virtual Data Center (VDC)
# @ strongmotioncenter.org
#
# The header included in the file obtained from the VDC, `vdc.chichi.QT063000_V`,
# was as follows:
#
# --------------------------------------------------------------------------------
# Uncorrected Acceleration  (Format v01.00 with 13 text lines) Orig: QT063000.ASC
# ChiChi,Taiwan Earthquake                September 20, 1999 17:47:15.670 UTC
# Hypocenter:23.860300 120.799500 10.33km(CWB)   Mw=7.6(CWB)
# Origin: 09/20/1999 17:47:15.670 UTC(CWB)Owner: CWB
# Station: TCU071 CWB 23.9855   120.7883  T071
# Site Geology: S_D (UBC 1997)
# Recorder: A900 S/N 63     ( 3 Chns of  3 at Sta) Sensor:  S/N
# Rcrd Start Time:09/20/99 17:46:59.000 UTC(Q=0) Recordid:
# Sta Cha: 1   UP      (Rcrdr Chan  1) Location:Free-field
# Raw record length: 160.000 sec  Uncor max= 416.0cm/s/s, at 31.900  sec
# Rec, filtered below:       Hz (periods over      secs), & above:       Hz
# Max:    416.015 cm/s/s , at 31.900  sec
# Values used in headers to denote parameter is unknown/unspecified: -999, -999.0
# 100 Integer-header values follow on 10 lines, Format  (10I8)
#        1       1       3     100       1    -999    -999    -999    -999    -999
#     -999    -999    -999    -999    -999    -999    -999    -999    -999    -999
#     -999       1       3    -999     182    -999      63       3       3       3
#     -999    -999    -999    -999    -999    -999    1999     263       9      20
#       17      47      15      67       0    -999    -999    -999    -999       1
#     -999    -999       0     400    -999    -999    -999    -999    -999    -999
#     -999    -999    -999    -999    -999    -999    -999    -999    -999    -999
#     -999    -999    -999    -999    -999    -999    -999    -999    -999    -999
#     -999    -999    -999    -999    -999    -999    -999    -999    -999    -999
#     -999    -999    -999    -999    -999    -999    -999    -999    -999    -999
# 100 Real-header values follow on 17 lines, Format (6F13.6)
#     23.985500   120.788300   187.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000    23.860300   120.799500    10.330000
#      7.600000  -999.000000  -999.000000  -999.000000    13.900000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000     0.005000
#      0.000000     0.000000  -999.000000  -999.000000  -999.000000  -999.000000
#    160.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000     2.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000   416.014893    31.900000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000  -999.000000  -999.000000
#   -999.000000  -999.000000  -999.000000  -999.000000
#   7  comment line(s) follow, each starting with a '|':
# | Station Map:http://scman.cwb.gov.tw/eqv3/accsta-e/acc-sta-e.htm
# | Original file, QT063000.ASC, created: January 08, 2001 18:00 UTC
# | http://www.cwb.gov.tw/V3.0e/index-e.htm
# | Converted to COSMOS format: July 18, 2001 22:37 UTC
# | http://www.cosmos-eq.org/cosmos_format_01.pdf
# | Retrieved from:
# | ftp://smdb.crustal.ucsb.edu/chichi/QT063000_V
# 32000  acceleration pts a  200 pts/sec, Units=cm/s/s     , Format=(1F14.9  )
# --------------------------------------------------------------------------------
#
# The remainder of the file was the acceleration data which has been copied
# to `chichi_QTO63000_V.csv`

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
    data_file = chichi_QT063000_V.csv
    gamma = 0.5
    beta = 0.25
    accel_fit_order = 3
  []
[]

[Executioner]
  type = Transient
  start_time = 5e-03
  end_time = 160
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

$ LS-DYNA Keyword file created by LS-PrePost 4.2 (Beta) - 04Feb2015(17:00)
$# Created on Mar-03-2015 (16:43:01)
*KEYWORD
*TITLE
$#                                                                         title
LS-DYNA keyword deck by LS-PrePost
*CONTROL_TERMINATION
$#  endtim    endcyc     dtmin    endeng    endmas      
45.0000000         0     0.000     0.000 1.0000E+8
*CONTROL_TIMESTEP
$#  dtinit    tssfac      isdo    tslimt     dt2ms      lctm     erode     ms1st
     0.0000      8.0         0     0.000     0.000         0         0         0
$#  dt2msf   dt2mslc     imscl    unused    unused     rmscl     
     0.000         0         0                         0.000
*DATABASE_NODOUT
$#      dt    binary      lcur     ioopt   option1   option2       
 5.0000E-3         3         0         1     0.000         0
*DATABASE_BINARY_D3PLOT
$#      dt      lcdt      beam     npltc    psetid      
0.10000000         0         0         0         0
$#   ioopt     
         0
*DATABASE_BINARY_D3THDT
$#      dt      lcdt      beam     npltc    psetid      
 5.0000E-3         0         0         0         0
*DATABASE_EXTENT_BINARY
$#   neiph     neips    maxint    strflg    sigflg    epsflg    rltflg    engflg
         0         0         3         1         1         1         1         1
$#  cmpflg    ieverp    beamip     dcomp      shge     stssz    n3thdt   ialemat
         0         0         0         1         1         1         2         1
$# nintsld   pkp_sen      sclp     hydro     msscl     therm    intout    nodout
         0         01.00000000         0         0         0STRESS    STRESS
$#    dtdt    resplt      
         0         0
*DATABASE_HISTORY_BEAM
$#     id1       id2       id3       id4       id5       id6       id7       id8
        12         5         0         0         0         0         0         0
*DATABASE_HISTORY_NODE
$#     id1       id2       id3       id4       id5       id6       id7       id8
        11        14        17        18       102         0         0         0
*BOUNDARY_PRESCRIBED_MOTION_NODE_ID
$#      id                                                               heading
         1X input
$#     nid       dof       vad      lcid        sf       vid     death     birth
       102         1         1         132.2000008         01.0000E+28     0.000
$#      id                                                               heading
         2Y input
$#     nid       dof       vad      lcid        sf       vid     death     birth
       102         2         1         232.2000008         01.0000E+28     0.000
$#      id                                                               heading
         3Z input
$#     nid       dof       vad      lcid        sf       vid     death     birth
       102         3         1         332.2000008         01.0000E+28     0.000
*PART
$#                                                                         title
Containment Structure
$#     pid     secid       mid     eosid      hgid      grav    adpopt      tmid
         1         1         1         0         0         0         0         0
*SECTION_BEAM_TITLE
Structural beamst
$#   secid    elform      shrf   qr/irid       cst     scoor       nsm   
         1         21.00000000         0         0     0.000     0.000
$#       a       iss       itt         j        sa       ist   
     0.000     0.000     0.000     0.000     0.000     0.000
*MAT_ELASTIC_TITLE
Containment concrete
$#     mid        ro         e        pr        da        db  not used        
         1 1.0000E-4 6.9000E+50.27800000     0.000     0.000         0
*PART
$#                                                                         title
Internal Structure
$#     pid     secid       mid     eosid      hgid      grav    adpopt      tmid
         2         1         2         0         0         0         0         0
*MAT_ELASTIC_TITLE
Internal structure concrete
$#     mid        ro         e        pr        da        db  not used        
         2 1.0000E-4 3.4500E+50.27800000     0.000     0.000         0
*DEFINE_CURVE_TITLE
Mass Damping
$#    lcid      sidr       sfa       sfo      offa      offo    dattyp      
       101         01.000000001.00000000     0.000     0.000         0
$#                a1                  o1  
               0.000          1.00000000
       1.000000e+021          1.00000000
*SET_PART_LIST_TITLE
Containment
$#     sid       da1       da2       da3       da4    solver      
         1     0.000     0.000     0.000     0.000MECH
$#    pid1      pid2      pid3      pid4      pid5      pid6      pid7      pid8
         1         0         0         0         0         0         0         0
*SET_PART_LIST_TITLE
Internal structure
$#     sid       da1       da2       da3       da4    solver      
         2     0.000     0.000     0.000     0.000MECH
$#    pid1      pid2      pid3      pid4      pid5      pid6      pid7      pid8
         2         0         0         0         0         0         0         0
*SET_PART_LIST_TITLE
Superstructure
$#     sid       da1       da2       da3       da4    solver      
       100     0.000     0.000     0.000     0.000MECH
$#    pid1      pid2      pid3      pid4      pid5      pid6      pid7      pid8
         1         2         0         0         0         0         0         0
*DAMPING_PART_MASS
$#     pid      lcid        sf      flag    
         1       101      3.20         0
*DAMPING_PART_MASS
$#     pid      lcid        sf      flag    
         2       101      3.20         0
*DAMPING_PART_STIFFNESS
$#     pid      coef    
         1-4.6675E-3
*DAMPING_PART_STIFFNESS
$#     pid      coef    
         2-4.6675E-3
*ELEMENT_BEAM_THICKNESS
$#   eid     pid      n1      n2      n3     rt1     rr1     rt2     rr2   local
       1       1     101       1     100       0       0       0       0       2
$#         parm1           parm2           parm3           parm4           parm5
     1400.000000   2.800000e+006   2.800000e+006   5.600000e+006      700.000000
       2       1       1       2     100       0       0       0       0       2
     1400.000000   2.800000e+006   2.800000e+006   5.600000e+006      700.000000
       3       1       2       3     100       0       0       0       0       2
     1400.000000   2.800000e+006   2.800000e+006   5.600000e+006      700.000000
       4       1       3       4     100       0       0       0       0       2
     1400.000000   2.800000e+006   2.800000e+006   5.600000e+006      700.000000
       5       1       4       5     100       0       0       0       0       2
     1400.000000   2.800000e+006   2.800000e+006   5.600000e+006      700.000000
       6       1       5       6     100       0       0       0       0       2
     1400.000000   2.800000e+006   2.800000e+006   5.600000e+006      700.000000
       7       1       6       7     100       0       0       0       0       2
     1400.000000   2.800000e+006   2.800000e+006   5.600000e+006      700.000000
       8       1       7       8     100       0       0       0       0       2
      990.000000   1.900000e+006   1.900000e+006   3.800000e+006      500.000000
       9       1       8       9     100       0       0       0       0       2
      990.000000   1.500000e+006   1.500000e+006   3.000000e+006      500.000000
      10       1       9      10     100       0       0       0       0       2
      990.000000   8.000000e+005   8.000000e+005   1.600000e+006      500.000000
      11       1      10      11     100       0       0       0       0       2
      990.000000   2.000000e+005   2.000000e+005   4.000000e+005      500.000000
      12       2     102      12     100       0       0       0       0       2
     2000.000000   1.100000e+006   1.100000e+006   2.200000e+006     1320.000000
      13       2      12      13     100       0       0       0       0       2
     2560.000000   1.200000e+006   1.200000e+006   2.400000e+006     1560.000000
      14       2      13      14     100       0       0       0       0       2
     2210.000000   1.200000e+006   1.200000e+006   2.400000e+006     1460.000000
      15       2      14      15     100       0       0       0       0       2
     1960.000000   1.300000e+006   1.300000e+006   2.600000e+006      730.000000
      16       2      15      16     100       0       0       0       0       2
     1740.000000   9.000000e+005   9.000000e+005   1.800000e+006      600.000000
      17       2      16      17     100       0       0       0       0       2
      780.000000   2.000000e+005   2.000000e+005   4.000000e+005      360.000000
      18       2      17      18     100       0       0       0       0       2
      190.000000     4000.000000     4000.000000     8000.000000     70.00000000
*ELEMENT_MASS
$#   eid     nid            mass     pid
       1       1          139.79       1
       2       2          127.61       1
       3       3          127.63       1
       4       4          127.63       1
       5       5          127.63       1
       6       6          127.63       1
       7       7          140.71       1
       8       8           91.78       1
       9       9           75.07       1
      10      10           64.72       1
      11      11            5.48       1
      12      12           85.52       2
      13      13           76.32       2
      14      14          193.22       2
      15      15          114.29       2
      16      16          263.40       2
      17      17           37.12       2
      18      18           25.17       2
*NODE
$#   nid               x               y               z      tc      rc  
       1           0.000           0.000     23.50000000       0       0
       2           0.000           0.000     43.79999924       0       0
       3           0.000           0.000     63.79999924       0       0
       4           0.000           0.000     83.80000305       0       0
       5           0.000           0.000      103.800003       0       0
       6           0.000           0.000      123.800003       0       0
       7           0.000           0.000      143.800003       0       0
       8           0.000           0.000      165.300003       0       0
       9           0.000           0.000      184.399994       0       0
      10           0.000           0.000      198.500000       0       0
      11           0.000           0.000      207.000000       0       0
      12           0.000           0.000      8.00000000       0       0
      13           0.000           0.000     13.00000000       0       0
      14           0.000           0.000     22.00000000       0       0
      15           0.000           0.000     33.50000000       0       0
      16           0.000           0.000     49.00000000       0       0
      17           0.000           0.000     61.00000000       0       0
      18           0.000           0.000     93.00000000       0       0
     100     10.00000000           0.000           0.000       7       7
     101           0.000           0.000           0.000       0       7
     102           0.000           0.000           0.000       0       7
*INCLUDE
1_acc.k
*END

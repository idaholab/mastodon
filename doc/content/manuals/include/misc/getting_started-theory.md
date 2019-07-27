## Introduction

Multi-hazard Analysis for STOchastic time-DOmaiN phenomena (MASTODON) is
a finite element application that aims at analyzing the response of 3-D
soil-structure systems to natural and man-made hazards such as
earthquakes, floods and fire. MASTODON currently focuses on the
simulation of seismic events and has the capability to perform extensive
'source-to-site' simulations including earthquake fault rupture,
nonlinear wave propagation and nonlinear soil-structure interaction
(NLSSI) analysis. MASTODON is being developed to be a dynamic
probabilistic risk assessment framework that enables analysts to not
only perform deterministic analyses, but also easily perform
probabilistic or stochastic simulations for the purpose of risk
assessment.

MASTODON is a [MOOSE-based](https://mooseframework.inl.gov) application and performs finite-element
analysis of the dynamics of solids, mechanics of interfaces and porous
media flow. It is equipped with effective stress space nonlinear hysteretic soil constitutive  model (I-soil), and a [u-p-U](https://lwrs.inl.gov/RisckInformed%20Safety%20Margin%20Characterization/INL-EXT-15-36735%20Advanced%20Seismic%20Fragility%20Modeling.pdf) formulation to couple solid and fluid, as well as structural materials such as reinforced concrete. It includes interface models that
simulate gapping, sliding and uplift at the interfaces of solid media
such as the foundation-soil interface of structures. Absorbing boundary models for the simulation of infinite or
semi-infinite domains, fault rupture model for seismic source
simulation, and the domain reduction method for the input of complex,
three-dimensional wave fields are incorporated. Since MASTODON is a MOOSE-based
application, it can efficiently solve problems using standard
workstations or very large high-performance computers.

This document describes the theoretical and numerical foundations of
MASTODON.

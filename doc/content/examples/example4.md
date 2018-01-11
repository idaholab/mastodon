# Example 4

This example describes a methodology on how to model frictional contact using the existing tools in MASTODON. Two brick elements are stacked vertically and a thin layer is modeled to describe the frictional contact (see \ref{fig:undeformedcontact}). Top and bottom layers use linear elastic material model whereas thin layer uses I-soil to approximate a contact behavior with elastic - nearly perfectly plastic shear behavior.

!listing examples/ex04/2BlockFriction_Isoilunit.i

!media media/examples/undeformedcontact.png width=80% margin-left=200px float=center id=fig:undeformedcontact caption=Undeformed shape of the numerical model.

The domain is loaded with gravity and top element is sheared on the top surface. At intermediate steps whereby the contact strength is yet not mobilized, no separation occurs between two elastic blocks as it is shown in \ref{fig:intermediate}.

!media media/examples/intermediate.png width=30% margin-left=200px float=center id=fig:intermediate caption=Intermediate step whereby the contact layer strength is not mobilized.

Once the contact layer is yielded, the upper block starts to slide on top of the lower block and thin layer effectively simulates the interface between two elastic layers (see \ref{fig:deformed}).

!media media/examples/deformed.png width=30% margin-left=200px float=center id=fig:deformed caption=Deformed shape of the numerical model after the contact is yielded.

\bibliographystyle{unsrt}
\bibliography{doc/bib/mastodon.bib}

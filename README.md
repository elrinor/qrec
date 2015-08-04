#About
QRec is a tool for automatic reconstruction of 3d models from engineering drawings. Current version is just a proof-of-concept and is not guaranteed to work on any drawings but the ones in the repository.

QRec implements a section identification scheme as described in `[1]`. For reconstruction of 3d solids a CSG-based method similar to the one described in `[2]` is used.

QRec takes input in dxf format. Currently it is possible only to display the reconstructed solid, not to save it.

`[1]` Gong, J., Zhang, H., Jiang, B., and Sun, J. 2010. Identification of sections from engineering drawings based on evidence theory. Comput. Aided Des. 42, 2 (Feb. 2010), 139-150. [DOI](http://dx.doi.org/10.1016/j.cad.2009.04.015)

`[2]` Dimri, J. and Gurumoorthy, B. 2005. Handling sectional views in volume-based approach to automatically construct 3D solid from 2D views. Comput. Aided Des. 37, 5 (Apr. 2005), 485-495. [DOI](http://dx.doi.org/10.1016/j.cad.2004.10.007)

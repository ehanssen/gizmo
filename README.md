# Gizmo


A little tool to help read angles from ultramicrotomes. Works with https://github.com/automated-ultramicrotomy/crosshair

# Manual
- sample and knife at 0 degres
- start Gizmo and wait for a few seconds until values show on the LCD (usually 15-20sec)
- Press the button which will reset all values to 0 degres (+/- a small value)
- Set up CrossHair and record value
- Rotate the samples and knife by the offsets given by CrossHair
- All ROT, TLT and KNF drift a little overtime so make sure you add the offset more than the pure values
  (eg initial ROT was 4 degres, CrossHair ask for 6 degres but the original value of Gizmo slowly drifted to 4.6 degres while doing alignement and crosshair set up so set 6.6 degres) 


# Cite

Eric Hanssen and Phil Francis, Ian Holmes Imaging Centre, Bio21 Institute, the University of Melbourne.
[doi:10.5281/zenodo.8223274](https://doi.org/10.5281/zenodo.8223274)

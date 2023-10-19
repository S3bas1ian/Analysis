# Analysis
Analysis scripts for Geant4 simulation fivi </br>

**count_rate2.cc** </br>
void count_rate2(std::string path, std::string particle, Double_t energy_min, std::string draw_opt, bool draw)</br>
Script calculates the count rate for each strip, the inner ASIC's, the wholed detectors and gives statistical informations, like mean, standard deviation and low and high quarter. The share of specific particles at the whole hits is also calculated
If drawing is enabled plots of the involved particles and the hits and count rate on each strip for each detector are plotted.
</br>
param: </br>
+ `path` to merged root file. Root file has to contain the merged hits and produced particles. The `merge_hits_produced.cc` script is available for merging.
+ `particle` constraints the analysis to the specific particle. Naming convention is the same of Geant4 (ECCE-EIC/geant4/tree/geant4-10.6-release/source/particles/) </br>
  When `particle = "all"` is used, all involved particles are considered
+ `energy_min` is the lower limit of energies that particles need to deposit to be considered a hit
+ `draw_opt`is a string that with the root drawing options for the 2d histogram for the count rates per strip. Typical options are "colz" or "candley"
+ `draw`plots are only created when draw is true. Otherwise only the stats are printed to the console </br>

</br>

**each_detector.cc** </br>
void each_detector(std::string path, std::string particle, Double_t energy_min) </br>
This script calculated 2d histogramms for the cluster size on each detector, the total hitzs per event for each detector as a 1d histogram as well as an overview about the involved particles. Mainly used for cluster sizes. </br>
The energy cut of 100keV is hardcoded </br>
param: </br>
+ `path` to root file. It is not neccessary to use a merged file here, because only the hits are relevant
+ `particle` consideres only the named particle. Using `"all"` considers all particles
+ `energy_min` is the lower limit of energies that particles need to deposit to be considered a hit </br>
</br>

**origin.cc**
void origin(std::string path, std::string particle, Double_t energy_min) </br>
Determines the origin of particles that triggered a detector. Plots the 100, 010 and 001 view on the origin. </br>
param: </br>
+ `path` to merged root file. Root file has to contain the merged hits and produced particles. The `merge_hits_produced.cc` script is available for merging.
+ `particle` consideres only the named particle. Using `"all"` considers all particles
+ `energy_min` is the lower limit of energies that particles need to deposit to be considered a hit</br>
</br>

**merge_hits_produced.cc**
void merge_hits_produced(std::string inputPath, std::string outputPath) </br>
Merges the trees hits and produced from one root file in a single tree in a new root file. This merged root file is compressed compared to the unmerged root file and is needed for all calculations of the origin of produced particles, 
e.g. in `origin.cc` or `edge_hit_xy_tgraph.cc`. </br>
path: </br>
+ `inputPath` path to root file, that has a hits and a produced tree
+ `outputPath` path to new root file where the merged tree/file is written to </br>
</br>

**edge_hit_xy_tgraph.cc**
void edge_hit_xy_tgraph(std::string path, std::string particle, Double_t energy_min) </br>
Creates plot with position of particles that hit the detector once ("one-side hitters"). Creates plot with position of "one-side hitters", where particles that have their origin inside the phantom have a different color than particles with their
origin inside the phantom. A third plot shows then the origin of the particles that are produced outside the phantom.</br>
param: </br>
+ `path` to merged root file. Root file has to contain the merged hits and produced particles. The `merge_hits_produced.cc` script is available for merging.
+ `particle` consideres only the named particle. Using `"all"` considers all particles
+ `energy_min` is the lower limit of energies that particles need to deposit to be considered a hit </br>
</br>

**edge_hit_strip.cc**
void edge_hit_strip(std::string path, std::string particle, Double_t energy_min) </br>
Creates 1d histogramm for each detector that contains the triggered strip id for "one-side hitters"
param: </br>
+ `path` to root file.
+ `particle` consideres only the named particle.
+ `energy_min` is the lower limit of energies that particles need to deposit to be considered a hit </br>
</br>


**stopping_power.cc**
void stopping_power(std::string path, std::string particle) </br>
Creates $\Delta E$ -E plots for specified particles </br>
param: </br>
+ `path` to root file.
+ `particle` consideres only the named particle. </br>
</br>

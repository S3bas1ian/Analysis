#include <TFile.h>
#include <TTree.h>
#include <string.h>

bool producedInPhantom(double x, double y, double z);

void origin(std::string path, std::string particle, Double_t energy_min)
{
    //open root file
    TFile *file = new TFile(path.c_str(), "read");
    TTree *hits = (TTree *)file->Get("hits");

    //variables to connect to
    char particle_name[128];
    Int_t event_number;
    Int_t det_id;
    Int_t strip_id;
    Double_t edep, energy;
    Double_t hits_x, hits_y, hits_z;
    Double_t produced_x, produced_y, produced_z;

    //connect root file to variables
    hits->SetBranchAddress("name", &particle_name);
    hits->SetBranchAddress("event", &event_number);
    hits->SetBranchAddress("Det_ID", &det_id);
    hits->SetBranchAddress("Strip_ID", &strip_id);
    hits->SetBranchAddress("edep", &edep);
    hits->SetBranchAddress("Hit_x", &hits_x);
    hits->SetBranchAddress("Hit_y", &hits_y);
    hits->SetBranchAddress("Hit_z", &hits_z);
    hits->SetBranchAddress("Hit_energy", &energy);
    hits->SetBranchAddress("Produced_x", &produced_x);
    hits->SetBranchAddress("Produced_y", &produced_y);
    hits->SetBranchAddress("Produced_z", &produced_z);

    std::vector<double> or_x, or_y, or_z; //origin x, y, z
    int or_phantom = 0; //counter particles origin phantom
    int or_outside = 0; //counter particles origin outside

    // preload stuff to speed things up
    //WARNING: Can cause problem in form of long loading times, if large data is loaded. 
    hits->LoadBaskets();

    int i = 0;
    int size = hits->GetEntries();

    while (i < size)
    {
        hits->GetEntry(i);
        int currEvent = event_number;
        bool sameEvent = true;
        bool counted = false;

        while (sameEvent && i < size) // loop through one full event, increase i while doing so
        {
            hits->GetEntry(i);

            if (event_number == currEvent)
            {
                // we are still looking at the right event
                if (edep > energy_min && !counted && (particle.compare(std::string(particle_name)) == 0 || particle.compare("all") == 0))
                {
                    if (producedInPhantom(produced_x, produced_y, produced_z))
                    {
                        or_phantom++;
                    }
                    else
                    {
                        or_outside++;
                    }
                    or_x.push_back(produced_x);
                    or_y.push_back(produced_y);
                    or_z.push_back(produced_z);
                    counted = true;
                }
                i += 1;
            }
            else
            {
                sameEvent = false;
            }
        }
    }

    // outputting
    cout << particle << " coming from phantom: " << or_phantom << " and from outside the phantom: " << or_outside << "\n";

    //plotting origin
    gStyle->SetLabelSize(59, "xyz");
    gStyle->SetTitleSize(70, "xyz");
    gStyle->SetTitleSize(0.09f, "t");
    // gStyle->SetCanvasDefW(2880);
    // gStyle->SetCanvasDefH(1800);
    gStyle->SetTitleFont(43, "xyz");
    gStyle->SetLabelFont(43, "xyz");
    gStyle->SetTickLength(0.04, "xyz");
    gStyle->SetLineWidth(4);
    gStyle->SetFrameLineWidth(4);
    gStyle->SetOptStat(0);
    gStyle->SetPadBottomMargin(0.17);
    gStyle->SetPadLeftMargin(0.11);
    gStyle->SetPadRightMargin(0.13);

    //plotting view 100
    auto c2 = new TCanvas((std::string("origin_100_particle==") + particle).c_str(), 
                            (std::string("origin_100_particle==") + particle + std::string(" (e_min= 100keV)")).c_str());
    c2->SetCanvasSize(2880, 1800);
    auto outsider_100 = new TGraph(or_y.size(), or_y.data(), or_z.data());
    outsider_100->SetTitle("yz-plane; y [mm]; z [mm]");
    outsider_100->Draw("AP");


    //plotting view 010
    auto c3 = new TCanvas((std::string("origin_010_particle==") + particle).c_str(), 
                            (std::string("origin_010_particle==") + particle + std::string(" (e_min= 100keV)")).c_str());
    c3->SetCanvasSize(2880, 1800);
    auto outsider_010 = new TGraph(or_x.size(), or_x.data(), or_z.data());
    outsider_010->SetTitle("xz-plane; x [mm]; z [mm]");
    outsider_010->Draw("AP");

    //plotting view 001
    auto c4 = new TCanvas((std::string("origin_001_particle==") + particle).c_str(), 
                            (std::string("origin_001_particle==") + particle + std::string(" (e_min= 100keV)")).c_str());
    c4->SetCanvasSize(2880, 1800);
    auto outsider_001 = new TGraph(or_x.size(), or_x.data(), or_y.data());
    outsider_001->SetTitle("xy-plane; x [mm]; y [mm]");
    outsider_001->Draw("AP");

}

// return if given coordinates are in phantom
bool producedInPhantom(double x, double y, double z)
{
    double Phantom_X = 0.0;
    double Phantom_Y = 0.0;
    double Phantom_Z = 0.0;
    double Phantom_DEPTH = 200;   // mm
    double Phantom_RADIUS = 100.; // mm

    if ((std::pow((x - Phantom_X), 2) + std::pow((z - Phantom_Z), 2) <= std::pow(Phantom_RADIUS, 2)) && z <= Phantom_DEPTH / 2 && z >= -Phantom_DEPTH / 2)
    {
        // position is within the Phantom
        return true;
    }
    else
    {
        return false;
    }
}
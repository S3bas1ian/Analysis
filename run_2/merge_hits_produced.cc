#include <TFile.h>
#include <TTree.h>
#include <string.h>

/*
programm to merge hits and produced particles. Generated output contains
one tree that has all information from "hits" + info where the particle
was created. Needed to determine if particle comes from phantom or is
produced in the tracker box
*/

class TreeWraper
{
private:
    std::unique_ptr<TFile> inFile;
    std::unique_ptr<TFile> outFile;

    TTree *hitsTree;
    TTree *producedTree;
    TTree *outputTree;

    // variables that are needed from the hitsTree
    char hits_particle_name_[128];
    Int_t hits_event_number_, hits_track_number_;
    Int_t hits_detId_, hits_stripId_;
    Double_t hits_edep_, hits_energy_;
    Double_t hits_time_;
    Double_t hits_x_, hits_y_, hits_z_;

    // variable that are needed from the produced tree
    char produced_particle_name_[128];
    Int_t produced_event_number_, produced_track_number_;
    Double_t produced_time_;
    Double_t produced_energy_;
    Double_t produced_x_, produced_y_, produced_z_;
    Double_t produced_px_, produced_py_, produced_pz_;

public:
    Long64_t size_p;

    TreeWraper(std::string inputPath, std::string outputPath)
    {
        /*
    using unique ptr to create the inputFile and outputFile
    extract the hits and produced trees from the input file
    */
        inFile = std::make_unique<TFile>(inputPath.c_str(), "READ");
        outFile = std::make_unique<TFile>(outputPath.c_str(), "RECREATE");
        cout << "treewrapper: files done" << endl;
        hitsTree = (TTree *)(inFile->Get("hits"));
        producedTree = (TTree *)(inFile->Get("produced"));
        outputTree = new TTree("hits", "hits");
        cout << "treewrapper: ttrees done" << endl;


        // set Branch for hits Tree
        hitsTree->SetBranchAddress("name", &hits_particle_name_);
        hitsTree->SetBranchAddress("event", &hits_event_number_);
        hitsTree->SetBranchAddress("Track_ID", &hits_track_number_);
        hitsTree->SetBranchAddress("Det_ID", &hits_detId_);
        hitsTree->SetBranchAddress("Strip_ID", &hits_stripId_);
        hitsTree->SetBranchAddress("edep", &hits_edep_);
        hitsTree->SetBranchAddress("energy", &hits_energy_);
        hitsTree->SetBranchAddress("time", &hits_time_);
        hitsTree->SetBranchAddress("x", &hits_x_);
        hitsTree->SetBranchAddress("y", &hits_y_);
        hitsTree->SetBranchAddress("z", &hits_z_);
        cout << "hits branches done" << endl;

        // set Branch for produced tree
        producedTree->SetBranchAddress("name", &produced_particle_name_);
        producedTree->SetBranchAddress("event", &produced_event_number_);
        producedTree->SetBranchAddress("track", &produced_track_number_);
        producedTree->SetBranchAddress("time", &produced_time_);
        producedTree->SetBranchAddress("energy", &produced_energy_);
        producedTree->SetBranchAddress("x", &produced_x_);
        producedTree->SetBranchAddress("y", &produced_y_);
        producedTree->SetBranchAddress("z", &produced_z_);
        producedTree->SetBranchAddress("px", &produced_px_);
        producedTree->SetBranchAddress("py", &produced_py_);
        producedTree->SetBranchAddress("pz", &produced_pz_);
        cout << "produced branches done" << endl;

        outputTree->Branch("name", &hits_particle_name_, "name/C");
        outputTree->Branch("event", &hits_event_number_, "event/I");
        outputTree->Branch("track", &hits_track_number_, "track/I");
        outputTree->Branch("Det_ID", &hits_detId_, "Det_ID/I");
        outputTree->Branch("Strip_ID", &hits_stripId_, "Strip_ID/I");
        outputTree->Branch("edep", &hits_edep_, "edep/D");
        outputTree->Branch("Hit_energy", &hits_energy_, "Hit_energy/D");
        outputTree->Branch("Hit_time", &hits_time_, "Hit_time/D");
        outputTree->Branch("Hit_x", &hits_x_, "Hit_x/D");
        outputTree->Branch("Hit_y", &hits_y_, "Hit_y/D");
        outputTree->Branch("Hit_z", &hits_z_, "Hit_z/D");

        outputTree->Branch("Produced_Energy", &produced_energy_, "Produced_Energy/D");
        outputTree->Branch("Produced_time", &produced_time_, "Produced_time/D");
        outputTree->Branch("Produced_x", &produced_x_, "Produced_x/D");
        outputTree->Branch("Produced_y", &produced_y_, "Produced_y/D");
        outputTree->Branch("Produced_z", &produced_z_, "Produced_z/D");
        outputTree->Branch("Produced_px", &produced_px_, "Produced_px/D");
        outputTree->Branch("Produced_py", &produced_py_, "Produced_py/D");
        outputTree->Branch("Produced_pz", &produced_pz_, "Produced_pz/D");

        
        size_p = producedTree->GetEntries();
        cout << "calculated size of produced" << endl;
    }

    ~TreeWraper()
    {
        cout << "destructor tree wrapper" << endl;
        if (inFile)
        {
            inFile->Close();
        }
        if(outFile){
            outFile->Close();
        }
    };

    Long64_t getEntriesHits()
    {
        return hitsTree->GetEntries();
    }

    Long64_t getEntriesProduced()
    {
        return producedTree->GetEntries();
    }

    void getEntryHits(Long64_t entry)
    {
        hitsTree->GetEntry(entry);
    }

    void getEntryProduced(Long64_t entry)
    {
        producedTree->GetEntry(entry);
    }

    Long64_t getEndIndex(int event, Long64_t startIndex)
    {
        Long64_t i = startIndex;
        while (i < this->size_p)
        {
            producedTree->GetEntry(i);
            if (produced_event_number_ != event)
            {
                return i;
            }
            i++;
        }
        cout << "Warning: Either end of file or no match found at index " << i << endl;
        return i;
    }

    Int_t getHitsEventNumber()
    {
        return hits_event_number_;
    }

    Int_t getProducedEventNumber()
    {
        return produced_event_number_;
    }

    Int_t getHitsTrackNumber()
    {
        return hits_track_number_;
    }

    Int_t getProducedTrackNumber()
    {
        return produced_track_number_;
    }

    Long64_t findProducedIndex(Int_t event, Int_t track, Long64_t startIndex, Long64_t endIndex)
    {
        Long64_t p = startIndex;
        while (p < endIndex)
        {
            getEntryProduced(p);
            if (produced_event_number_ == event && produced_track_number_ == track)
            {
                return p;
            }
            else
            {
                p++;
            }
        }
        std::cerr << "Error finding particle at index " << p << "\n";
        abort();
    }

    void fill(){
        outputTree->Fill();
    }

    void write(){
        outputTree->Write();
    }
};

void merge_hits_produced(std::string inputPath, std::string outputPath)
{
    TreeWraper wrapper = TreeWraper(inputPath, outputPath);
    Long64_t h = 0; // iterater index for hits branch
    Long64_t size_h = wrapper.getEntriesHits();
    wrapper.getEntryHits(h);
    Long64_t startIndex = 0;
    Long64_t endIndex = wrapper.getEndIndex(wrapper.getHitsEventNumber(), startIndex);  //end Index is the produced index of the last entrie that belongs to a hit event

    while (h < size_h)
    {
        wrapper.getEntryHits(h);
        int currEvent = wrapper.getHitsEventNumber();
        bool sameEvent = true;

        while (sameEvent && h < size_h)
        {
            wrapper.getEntryHits(h);

            if (currEvent == wrapper.getHitsEventNumber())
            {
                //find fitting produced inbetween start and end index
                Long64_t p = wrapper.findProducedIndex(currEvent,
                                                        wrapper.getHitsTrackNumber(),
                                                        startIndex,
                                                        endIndex);
                wrapper.getEntryProduced(p);    //get content of fitting produced
                wrapper.fill(); //fill all the (merged) information.

                h++;    //go to next entrie in the same event block
            }
            else
            {
                //one event block is finished
                sameEvent = false;
                startIndex = endIndex; //we start now there where we ended
                endIndex = wrapper.getEndIndex(wrapper.getHitsEventNumber(), startIndex); //calculating the new end index
            }
        }
    }
    wrapper.write();    //create merged root file
    std::cout << "Done!" << std::endl;
}
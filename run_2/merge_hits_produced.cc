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
        cout << "treewrapper: ttrees done" << endl;


        // set Branch for hits Tree
        hitsTree->SetBranchAddress("name", &hits_particle_name_);
        hitsTree->SetBranchAddress("event", &hits_event_number_);
        hitsTree->SetBranchAddress("Track_ID", &hits_track_number_);
        hitsTree->SetBranchAddress("Det_ID", &hits_detId_);
        hitsTree->SetBranchAddress("Strip_ID", &hits_stripId_);
        hitsTree->SetBranchAddress("edep", &hits_edep_);
        hitsTree->SetBranchAddress("energy", &hits_energy_);
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

        // preloads 2gb to increase speed
        hitsTree->LoadBaskets();
        cout << "hits Tree baskets loaded" << endl;
        producedTree->LoadBaskets();
        cout << "produced Tree baskets loaded" << endl;
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
        cout << "entered getEndIndex" << endl;
        Long64_t i = startIndex;
        while (i < this->size_p)
        {
            cout << "entered while loop in getEndIndex" << endl;
            producedTree->GetEntry(i);
            if (produced_event_number_ != event)
            {
                cout << "found end index" << endl;
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
            cout << "entered while loop findProduucedIndex" << endl;
            getEntryProduced(p);
            if (produced_event_number_ == event && produced_track_number_ == track)
            {
                cout << "found produced index" << endl;
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
};

void merge_hits_produced(std::string inputPath, std::string outputPath)
{
    TreeWraper wrapper = TreeWraper(inputPath, outputPath);
    cout << "finished wrapper" << endl; 
    Long64_t h = 0; // iterater index for hits branch
    Long64_t size_h = 200;//wrapper->getEntriesHits();
    wrapper.getEntryHits(h);
    Long64_t startIndex = 0;
    Long64_t endIndex = wrapper.getEndIndex(wrapper.getHitsEventNumber(), startIndex);
    cout << "first time endIndex" << endl;

    while (h < size_h)
    {
        cout << "entered loop at h= " << h << endl;
        wrapper.getEntryHits(h);
        int currEvent = wrapper.getHitsEventNumber();
        bool sameEvent = true;

        while (sameEvent && h < size_h)
        {
            wrapper.getEntryHits(h);
            cout << "in second while loop" << endl;

            if (currEvent == wrapper.getHitsEventNumber())
            {

                Long64_t p = wrapper.findProducedIndex(currEvent,
                                                        wrapper.getHitsTrackNumber(),
                                                        startIndex,
                                                        endIndex);
                wrapper.getEntryProduced(p);
                std::cout << "Hits [event, track]: " << wrapper.getHitsEventNumber() << "  " 
                << wrapper.getHitsTrackNumber() << "  Produced [, ]: " 
                << wrapper.getProducedEventNumber() << "  " 
                << wrapper.getProducedTrackNumber() << "\n";

                h++;
            }
            else
            {
                sameEvent = false;
                startIndex = endIndex;
                endIndex = wrapper.getEndIndex(wrapper.getHitsEventNumber(), startIndex);
            }
        }
    }
}
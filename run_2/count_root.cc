#include "TFile.h"
#include "TGraph.h"
#include "TTree.h"

#include <string>
#include <vector>
#include <memory>

/*  Create an object to control access to the TTree, since you want to
 *  pass around all of these variables as a group
 */
class TreeWrapper{
    private: 
        //Use unique_ptr instead of raw pointer wherever possible.
        std::unique_ptr<TFile> file_;
        std::unique_ptr<TFile> output_File;
        //std::shared_ptr<TFile> file_;
        
        //ROOT's does not support the use of smart pointers here
        TTree* tree_;
        TTree* output_Tree;
        
        char particleName_[128];
        /*  Use ROOT's data types Int_t & Double_t for improved
         * portability
         */
        Int_t eventNumber_;
        Int_t detId_;
        Int_t stripId_;
        Double_t edep_;
        Double_t time_;

        Double_t av_delte_time, stdv; 
        Int_t out_det_id, out_strip_id;
        
    public:
        /*  This is just all the setup you had before, moved into the
         *  constructor. Let you define a path, rather than forcing it
         *  to be hardcoded, for better reuse.
         */
        TreeWrapper(std::string path){
            file_ = std::make_unique<TFile>(path.c_str(), "read");
            file_ = std::make_unique<TFile>("data/delta_time.root", "recreate");
            output_Tree = new TTree("delta_time", "delta_time");

            output_Tree->Branch("av_delta_time", &av_delte_time, "av_delta_time/D");
		    output_Tree->Branch("stdv", &stdv, "stdv/D");
            output_Tree->Branch("det_id", &out_det_id, "out_det_id/I");
		    output_Tree->Branch("strip_id", &out_strip_id, "out_strip_id/I");
            //file_ = std::make_shared<TFile>(path.c_str(), "read");
            
            //Slightly safer method of getting access to the TTree
            file_->GetObject("hits", tree_);
            
            tree_->SetBranchAddress("name", &particleName_);
            tree_->SetBranchAddress("event", &eventNumber_);
            tree_->SetBranchAddress("Det_ID", &detId_);
            tree_->SetBranchAddress("Strip_ID", &stripId_);
            tree_->SetBranchAddress("edep", &edep_);
            tree_->SetBranchAddress("time", &time_);
            
            tree_->LoadBaskets();
        }
        
        ~TreeWrapper(){
            if(file_){
                file_->Close();
            }
        };
        
        //Provide access to the TTree methods you need to use
        Long64_t getEntries(){
            return tree_->GetEntries();
        }
        
        void getEntry(Long64_t entry){
            tree_->GetEntry(entry);
        }
        
        //Provide access to the data you want to be able to see
        std::string getName(){
            return std::string(particleName_);
        }
        
        Int_t getEventNum(){
            return eventNumber_;
        }
        
        Int_t getDetID(){
            return detId_;
        }
        
        Int_t getStripID(){
            return stripId_;
        }
        
        Double_t getEdep(){
            return edep_;
        }
        
        Double_t getTime(){
            return time_;
        }

        void enterOutput(Double_t av_delta_time, Double_t stdv, Int_t det_id, Int_t strip_id)
	{
		this->av_delte_time = av_delta_time;
		this->stdv = stdv;
		out_det_id = det_id;
		out_strip_id = strip_id;

		output_Tree->Fill();
	}
};

/*  Change return type, since you want to return just a pair of values
 *  Change parameters to accept the TTree wrapper class, other needed
 *  values
 */
std::pair<Double_t, Double_t> get_Mean_and_Stdev(Int_t detID, Int_t stripID, TreeWrapper& tree, Double_t psPerEvent){
    Long64_t size = tree.getEntries();
    
    //Since you use the vector in this method, declare it in this method
    std::vector<Double_t> times;
    
    /*  Looks basically the same, but replace `this' with your tree
     *  wrapper object
     */
    for (Long64_t i = 0 ; i < size ; ++i){
        tree.getEntry(i);
        
        if (tree.getDetID() == detID && tree.getStripID() == stripID){
            times.push_back(psPerEvent * tree.getEventNum() + tree.getTime());
        }
    }
    
    std::sort(times.begin(), times.end());
    
    std::vector<Double_t> delta_times;
    //auto h = new TH1D ("hist", "delta time", 100, 0, 2);
    
    /*  Change the offset by 1 to the start, rather than the end, since
     *  the size type is unsigned, and otherwise this fails at size 0.
     */
    for (std::vector<Double_t>::size_type i = 1 ; i < times.size() ; ++i){
        Double_t dt = times[i] - times[i - 1];
        delta_times.push_back(dt);
        //h->Fill(dt);
    }
    
    //Declare the iterators in a slightly more concise way
    Double_t sum = std::accumulate(delta_times.begin(), delta_times.end(), 0.0);
    
    Double_t mean = sum / delta_times.size();
    
    //Can use accumulate here again if you want to be fancy
    Double_t stdev = TMath::Sqrt(
        std::accumulate(
            delta_times.begin(),
            delta_times.end(),
            0.0,
            [mean](Double_t sum, Double_t elem){return sum + (elem - mean) * (elem - mean);}
        ) / delta_times.size()
    );
    
    /*  Foreach should just compile to this anyway, if you don't care
     *  about being fancy
     */
    /* Double_t stdev2 = 0.0;
    
    for (auto dt : delta_times){
        stdev2 += (dt - mean) * (dt - mean);
    }
    stdev2 /= delta_times.size();
    stdev2 = TMath::Sqrt(stdev2);
    
    std::cout << stdev << std::endl;
    std::cout << stdev2 << std::endl;
    std::cout << stdev2 - stdev << std::endl;
    */
    
    /*  Helper method, rather than needing to declare stuff. This also
     *  allows C++ to do some optimization, this will actually be
     *  constructed in the place where the return value would be moved
     *  to, rather than in this method, then moved when the method
     *  returns.
     */
    return std::make_pair(mean, stdev);
}

/*  Move the main method to the bottom, to avoid needing prototypes,
 *  since ROOT, like the preprocessor, reads files top to bottom
 */
void count_root(std::string path)
{

    //Int_t detectors[] = {0, 1, 7, 8};
    //Int_t strips[] = {0, 1, 2, 200, 555, 700, 1021, 1022, 1023};
    
    //This does not seem to be used?
    Double_t e_min = 100000; // unit eV
    
    Double_t psPerEvent = 1e4;  // unit ps
    
    //TreeWrapper input("data/final_output/output3.root");
    //TreeWrapper input(path);
    TreeWrapper input = TreeWrapper(path);
    //auto input = std::make_unique<TreeWrapper>(path);
    
    //Loop is unchanged, just cleaned up names for readability
    for (Int_t det = 0; det<1; det++){
        for (Int_t strip= 0; strip<400; strip+=4){
            //use auto here to avoid needing to write out the full type
            //std::pair<Double_t, Double_t>
            auto rval = get_Mean_and_Stdev(det, strip, input, psPerEvent);
            
            input.enterOutput(rval.first, rval.second, det, strip);
        }
    }
}

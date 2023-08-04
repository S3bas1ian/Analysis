#include "TFile.h"
#include "TGraph.h"
#include "TTree.h"

#include <string>
#include <vector>
#include <memory>

/*  Create an object to control access to the TTree, since you want to
 *  pass around all of these variables as a group
 */
class TreeWrapper
{
private:
	// Use unique_ptr instead of raw pointer wherever possible.
	std::unique_ptr<TFile> file_;
	std::unique_ptr<TFile> delta_time_file;
	// std::shared_ptr<TFile> file_;

	// ROOT's does not support the use of smart pointers here
	TTree *tree_;
	TTree *output_tree;

	//TH2D* hist;

	char particleName_[128];
	/*  Use ROOT's data types Int_t & Double_t for improved
	 * portability
	 */
	Int_t eventNumber_;
	Int_t detId_;
	Int_t stripId_;
	Double_t edep_;
	Double_t time_;

	Double_t av_delta_time, stdv, min, max, median, l_quarter, h_quarter;
	Int_t out_det_id, out_strip_id;

public:
	/*  This is just all the setup you had before, moved into the
	 *  constructor. Let you define a path, rather than forcing it
	 *  to be hardcoded, for better reuse.
	 */
	TreeWrapper(std::string path)
	{
		file_ = std::make_unique<TFile>(path.c_str(), "read");
		//delta_time_file = std::make_unique<TFile>("data/hist.root", "recreate");
		//output_tree = new TTree("delta_time", "delta_time");

		// output_tree->Branch("av_delta_time", &av_delta_time, "av_delta_time/D");
		// output_tree->Branch("stdv", &stdv, "stdv/D");
		// output_tree->Branch("min", &min, "min/D");
		// output_tree->Branch("max", &max, "max/D");
		// output_tree->Branch("median", &median, "median/D");
		// output_tree->Branch("l_quarter", &l_quarter, "l_quarter/D");
		// output_tree->Branch("h_quarter", &h_quarter, "h_quarter/D");
		// output_tree->Branch("det_id", &out_det_id, "out_det_id/I");
		// output_tree->Branch("strip_id", &out_strip_id, "out_strip_id/I");

		// file_ = std::make_shared<TFile>(path.c_str(), "read");

		// Slightly safer method of getting access to the TTree
		file_->GetObject("hits", tree_);

		tree_->SetBranchAddress("name", &particleName_);
		tree_->SetBranchAddress("event", &eventNumber_);
		tree_->SetBranchAddress("Det_ID", &detId_);
		tree_->SetBranchAddress("Strip_ID", &stripId_);
		tree_->SetBranchAddress("edep", &edep_);
		tree_->SetBranchAddress("time", &time_);

		tree_->LoadBaskets();
	}

	~TreeWrapper()
	{
		if (file_)
		{
			file_->Close();
		}

		if(delta_time_file){
			delta_time_file->Close();
		}
	};

	// Provide access to the TTree methods you need to use
	Long64_t getEntries()
	{
		return tree_->GetEntries();
	}

	/*void Fill(Double_t x, Double_t y){
		hist->Fill(x, y);
	}*/

	void getEntry(Long64_t entry)
	{
		tree_->GetEntry(entry);
	}

	/*void Write(){
		auto c1 = new TCanvas("c1", "$Delta t$ for each strip");
		hist->SetXTitle("$Delta t$ [ms]");
		hist->SetYTitle("strip");
		hist->Draw();
		c1->SaveAs("delta_time.png");

		delta_time_file->WriteObject(&hist, "Delta_time");

	}*/
	

	// Provide access to the data you want to be able to see
	std::string getName()
	{
		return std::string(particleName_);
	}

	Int_t getEventNum()
	{
		return eventNumber_;
	}

	Int_t getDetID()
	{
		return detId_;
	}

	Int_t getStripID()
	{
		return stripId_;
	}

	Double_t getEdep()
	{
		return edep_;
	}

	Double_t getTime()
	{
		return time_;
	}

	// void enterOutput(Double_t av_delta_time, Double_t stdv, Double_t min, Double_t max, Double_t median, Double_t l_quarter, Double_t h_quarter, Int_t det_id, Int_t strip_id)
	// {
	// 	this->av_delta_time = av_delta_time;
	// 	this->stdv = stdv;
	// 	this->min = min;
	// 	this->max = max;
	// 	this->median = median;
	// 	this->l_quarter = l_quarter;
	// 	this->h_quarter = h_quarter;
	// 	out_det_id = det_id;
	// 	out_strip_id = strip_id;

	// 	output_tree->Fill();
	// }
};

/*  Change return type, since you want to return just a pair of values
 *  Change parameters to accept the TTree wrapper class, other needed
 *  values
 */
void get_Boxplot_and_Stdv(Int_t detID, Int_t stripID, TreeWrapper &tree, TH2D& hist, Double_t psPerEvent)
{
	Long64_t size = tree.getEntries();

	// Since you use the vector in this method, declare it in this method
	std::vector<Double_t> times;

	/*  Looks basically the same, but replace `this' with your tree
	 *  wrapper object
	 */
	for (Long64_t i = 0; i < size; ++i)
	{
		tree.getEntry(i);

		if (tree.getDetID() == detID && tree.getStripID() == stripID)
		{
			times.push_back(psPerEvent * tree.getEventNum() + tree.getTime());
		}
	}

	std::sort(times.begin(), times.end());

	std::vector<Double_t> delta_times;
	std::sort(delta_times.begin(), delta_times.end());
	// auto h = new TH1D ("hist", "delta time", 100, 0, 2);

	/*  Change the offset by 1 to the start, rather than the end, since
	 *  the size type is unsigned, and otherwise this fails at size 0.
	 */
	for (std::vector<Double_t>::size_type i = 1; i < times.size(); ++i)
	{
		Double_t dt = times[i] - times[i - 1];
		delta_times.push_back(dt);
		hist.Fill(dt/1e9 ,stripID);
		// h->Fill(dt);
	}

	// Declare the iterators in a slightly more concise way
	// Double_t sum = std::accumulate(delta_times.begin(), delta_times.end(), 0.0);

	// Double_t mean = sum / delta_times.size();

	// // Can use accumulate here again if you want to be fancy
	// Double_t stdev = TMath::Sqrt(
	// 	std::accumulate(
	// 		delta_times.begin(),
	// 		delta_times.end(),
	// 		0.0,
	// 		[mean](Double_t sum, Double_t elem)
	// 		{ return sum + (elem - mean) * (elem - mean); }) /
	// 	delta_times.size());

	// Double_t min = delta_times[0];
	// Double_t max = delta_times[1];

	// int s = delta_times.size();
	// Double_t median = delta_times[s/2];
	// Double_t l_quarter = delta_times[s/4];
	// Double_t h_quarter = delta_times[3*s/4];

	// std::vector<Double_t> output;

	// output.push_back(mean);
	// output.push_back(stdev);
	// output.push_back(min);
	// output.push_back(max);
	// output.push_back(median);
	// output.push_back(l_quarter);
	// output.push_back(h_quarter);


	/*  Helper method, rather than needing to declare stuff. This also
	 *  allows C++ to do some optimization, this will actually be
	 *  constructed in the place where the return value would be moved
	 *  to, rather than in this method, then moved when the method
	 *  returns.
	 */
	// return output;
}

/*  Move the main method to the bottom, to avoid needing prototypes,
 *  since ROOT, like the preprocessor, reads files top to bottom
 */
void count_rate(std::string path, Int_t det)
{
	Int_t detectors[] = {0, 1, 6, 7};
	Int_t strips[] = {0, 1, 2, 200, 555, 700, 1021, 1022, 1023};

	Double_t psPerEvent = 1e4; // unit ps

	TreeWrapper input = TreeWrapper(path);

	auto hist = TH2D("Delta_time", "Time between two hits for each strip", 500, 0, 0.5, 1024, 0, 1023);

	// Loop is unchanged, just cleaned up names for readability
	
		for (Int_t strip = 0; strip < 1024; strip += 4)
		{
			// use auto here to avoid needing to write out the full type
			// std::pair<Double_t, Double_t>
			get_Boxplot_and_Stdv(det, strip, input, hist, psPerEvent);

			//input.enterOutput(rval[0], rval[1], rval[2], rval[3], rval[4], rval[5], rval[6], det, strip);

			// std::cout << "------------ detector: " << det << " and strip: " << strip << "------------" << std::endl;
			// std::cout << "average [ps]: " << rval[0] << std::endl;
			// std::cout << "std deviation [ps]: " << rval[1] << std::endl;

			// // add a blank line for easier visual separation between data
			// std::cout << std::endl;
		}
	
	auto c1 = new TCanvas("c1", "colz");
	hist->SetXTitle("$Delta t$ [ms]");
	hist->SetYTitle("strip");
	hist.DrawCopy("ColZ");
	c1->SaveAs("count_rate_colz.png");

	auto c2 = new TCanvas("c2", "candley2");
	hist.DrawCopy("candley2");
	c2->SaveAs("count_rate_candley.png");

	//input.Write();
}

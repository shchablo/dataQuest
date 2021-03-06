void SigmoideFitter(TGraphErrors* efficiency) {

  float HVmin, HVmax; 

  HVmax = HV;

 TF1* sigmoid = new TF1("sigmoid","[0]/(1+exp([1]*([2]-x)))",HVmin - 100, HVmax + 100);
 sigmoid->SetParName(0,"#epsilon_{max}");
 sigmoid->SetParName(1,"#lambda");
 sigmoid->SetParName(2,"HV_{50%}");
 sigmoid->SetParameter(0,0.99);
 sigmoid->SetParameter(1,0.01);
 sigmoid->SetParameter(2,7500);

  efficiency->Fit(sigmoid);


 sTitle = sTitle + "; HVeff; Eff";
 TH1D* Plotter = new TH1D("Plotter", sTitle.c_str(), 1, HVmin-100,HVmax+100);

  Plotter->SetStats(0);

  Plotter->SetMinimum(0.);
  Plotter->SetMaximum(1.05);
//  Plotter->SetMinimum(0);
//  Plotter->SetMaximum(1800);

  Plotter->Draw();

  efficiency->Draw("P");

 //gPad->SetLogy(1);

  TLegend *leg = new TLegend(0.608739,0.3024784,0.8085924,0.4023343,NULL,"brNDC");
  leg->SetBorderSize(0);
  leg->SetTextFont(62);
  leg->SetTextSize(0.04);
  leg->SetLineColor(1);
  leg->SetLineStyle(1);
  leg->SetLineWidth(1);
  leg->SetFillColor(0);
  leg->SetFillStyle(1001);

  
  leg->AddEntry(efficiency,"Efficiency","P");

  //  leg->Draw();

  double p1 = sigmoid->GetParameter(0);
  double p2 = sigmoid->GetParameter(1);
  double p3 = sigmoid->GetParameter(2);
   	

  double uLimit = HVmax, lLimit = HVmin;

  TLatex* ltx = new TLatex();
  ltx->SetTextSize(0.04);

  double knee = p3 - log(1/0.99-1)/p2;
  cout << "Val at knee " << sigmoid->Eval(knee) << " Val at knee/p1 = " << sigmoid->Eval(knee)/p1 << endl;

  double WP = knee+25;

  TLine* lWP = new TLine(WP, 0., WP, 1);
  lWP->SetLineStyle(2);
  lWP->Draw();

  double add = (uLimit-lLimit)/11.; 
  ltx->DrawLatex(WP-8*add, 0.42, Form("Eff(WP) = %.3f", sigmoid->Eval(WP)));
  ltx->DrawLatex(WP-8*add, 0.35, Form("WP = %.3f V", WP));
  ltx->DrawLatex(WP-8*add, 0.27, Form("knee = %.3f V", knee));
  ltx->DrawLatex(WP-8*add, 0.20, Form("HV(50%) = %.3f V", p3));


  TLine* plateau = new TLine(lLimit-50, p1, uLimit+50, p1);
  plateau->SetLineStyle(2);
  plateau->Draw();

  add = (uLimit-lLimit)/11.;
  
  if ((knee - lLimit) < (uLimit-lLimit)*(3/11.)) add = knee + add;
  else add = lLimit+add;

  ltx->DrawLatex(add, p1+0.02, Form("plateau = %.3f", p1));

  cout << "knee = " << knee << endl;

  string outfile = "results/Sigmoids/Efficiency";
  string outfile_png = outfile + "" + fileIn + ".png";
  string outfile_pdf = outfile + "" + fileIn + ".pdf";

  gPad->SaveAs(outfile_png.c_str());
  gPad->SaveAs(outfile_pdf.c_str());

}

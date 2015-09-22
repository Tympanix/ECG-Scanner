// Initialize functions
void filterNextData();
void updateThresholds();
void updateLowHighMiss(int AVG);
int calculateRRAVG1();
int calculateRRAVG2();
void updateNewRPeak(Peak peak);
Peak findPeakSearchback();
void searchBack();
void foundRPeak(Peak peak);
void updateNoRPeak(Peak peak);
void updateGUI(Peak peak);
int calculatePulse();

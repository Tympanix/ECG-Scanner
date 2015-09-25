void checkHeartAttack();
int ignorePeakSpike(Peak peak);
void filterNextData();
void updateThresholds();
void updateLowHighMiss(int AVG);
int calculateRRAVG1();
int calculateRRAVG2();
void updateNewRPeak(Peak peak);
Peak findPeakSearchback();
void searchBack(Peak peak);
void foundRPeak(Peak peak);
void checkMissedHeartbeat();
void updateNoRPeak(Peak peak);
void updateGUI(Peak peak);
int updatePulse();
int debug(time);


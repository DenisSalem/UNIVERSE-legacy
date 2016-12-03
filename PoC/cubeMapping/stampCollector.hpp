class StampCollector {
  public:
    StampCollector(int LoD);
    float * GetCone();

  private:
    void SetCone();
    int scale;
    int area;
    float * cone;
};

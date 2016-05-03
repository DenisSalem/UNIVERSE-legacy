class StampCollector {
  public:
    StampCollector(int LoD);
    float * GetCone();

  private:
    void SetCone();
    int scale;
    float * cone;
};

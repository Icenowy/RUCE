#define _Wave _C(CDSP2_Wave, _, _T1)
#define _List_Int CDSP2_List_Int
#define _List_Sinusoid _C(CSVP_List_Sinusoid, _, _T1)
#define _List_DataFrame _C(CDSP2_List_DataFrame, _, _T1)
#define _Sinusoid _C(CSVP_Sinusoid, _, _T1)
#define _DataFrame _C(CDSP2_DataFrame, _, _T1)

RClass(_RTClassName)
{
    RInherit(RObject);
    
    //Public
    _List_Int      PulseList;
    _List_Sinusoid SinuList;
    
    //Private
    _List_DataFrame PhseList;
    void* Wave;
    int LastPosition;
    _T1 LastPositionP;
    int InitPosition;
    _Sinusoid LastSinusoid;
    _Sinusoid InitSinusoid;
    _DataFrame LastPhse;
    _DataFrame InitPhse;
};

RTMethod(void, CSVP_SinusoidItersizer, From, _RTClassName* Sorc);
RTMethod(void, CSVP_SinusoidItersizer, Clear);

RTMethod(void, CSVP_SinusoidItersizer, SetWave, void* Sorc);
RTMethod(void, CSVP_SinusoidItersizer, SetInitPhase, _DataFrame* Sorc);
RTMethod(void, CSVP_SinusoidItersizer, SetPosition, int Position);
RTMethod(int , CSVP_SinusoidItersizer, GetPosition);

RTMethod(void, CSVP_SinusoidItersizer, IterNextTo, int Position);
RTMethod(void, CSVP_SinusoidItersizer, PrevTo, int Position);

RTMethod(void, CSVP_SinusoidItersizer, Add, _Sinusoid* Sorc, int Position);

#undef  _Wave
#undef  _List_Int
#undef  _List_Sinusoid
#undef  _List_DataFrame
#undef  _Sinusoid
#undef  _DataFrame


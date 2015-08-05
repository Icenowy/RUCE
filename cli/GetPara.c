#include "GetPara.h"
#include "Pitchbend.h"
#include <stdio.h>
#include <RUtil2.h>

void PrintUsage(char *FileName)
{
    printf("Usage: %s <input file> <output file> <pitch percent>\n"
           "                <velocity> [<flags> [<offset> <length require>\n"
           "                [<fixed length> [<end blank> [<volume>\n"
           "                [<modulation> [<pitch bend>...]]]]]]]\n", FileName);
}

int RUCE_ParsePara(RUCE_UnitParam* Dest, int argc, char** argv)
{
    int i;
    int Ret = 1;
    int CLV = argc;
    int EnablePitchConv = 1;
    float Tempo;
    float Freq;
    
    String PP, PBD;
    RNew(String, & PP, & PBD);
    if(CLV > 13)
    {//Resolving Cadencii Style PitchBend
        String_SetChars(& PBD, argv[12]);
        String QChar;
        RNew(String, & QChar);
        String_SetChars(& QChar, "Q");
        int QPos = InStrRev(& PBD, & QChar);
        RDelete(& QChar);
        if(QPos != -1)
        {
            Tempo = atof(String_GetChars(& PBD) + QPos + 1);
            if(Tempo <= 0)
            {
                fprintf(stderr, "[Error] Invalid tempo as '%s'.\n", argv[12]);
                Ret = 0;
                goto RExit;
            }
            String_SetChars(& PP, argv[3]);
            Freq = Tune_SPNToFreq_Float(& PP);
            
            for(i = 0; i < (CLV - 13); ++ i)
            {
                float PitchCent = atof(argv[13 + i]);
                PMatch_Float_Float_AddPair(&Dest -> Freq,
                    Tune_BeatToTime_Float(Tempo, (((float)i) / 96.0f)),
                    Tune_AddCentToFreq_Float(Freq, PitchCent));
            }
         /* printf("Detected Cadencii Style PitchBend,
                Tempo = %f, Points = %d\n", Tempo, CLV - 13); */
            CLV = 12;
            EnablePitchConv = 0;
        }
    }
    switch(CLV)
    {
        case 14:
            String_SetChars(& PP, argv[3]);
            String_SetChars(& PBD, argv[13]);
            Tempo = atof(argv[12] + 1);
            if(Tempo <= 0)
            {
                fprintf(stderr, "[Error] Invalid tempo as '%s'.\n", argv[12]);
                Ret = 0;
                break;
            }
            int DataNum = RUCE_Pitchbend_GetLength(& PBD);
            short* Data = RAlloc(DataNum * sizeof(short));
            Freq = Tune_SPNToFreq_Float(& PP);
            RUCE_Pitchbend_Decode(Data, & PBD);
            for(i = 0; i < DataNum; ++ i)
            {
                Data[i] = Data[i] > 2048 ? Data[i] - 4096 : Data[i];
                PMatch_Float_Float_AddPair(& Dest -> Freq,
                    Tune_BeatToTime_Float(Tempo, (((float)i) / 96.0f)),
                    Tune_AddCentToFreq_Float(Freq, Data[i]));
            }
            RFree(Data);
            /* printf("Detected UTAU Style PitchBend, Tempo = %f
                 , Points = %d\n", Tempo, DataNum);*/
            EnablePitchConv = 0; // Disable standalone pitch conv.
            CLV -= 2;
        
        case 13:
        case 12:
            Dest -> Modulation = atof(argv[11]);
            -- CLV;
            
        case 11:
            Dest -> Volume = atof(argv[10]);
            if(Dest -> Volume < 0.0f)
            {
                Ret = 0;
                fprintf(stderr, "[Error] Invalid volume as '%s'.\n", argv[10]);
                break;
            }
            -- CLV;
            
        case 10:
            Dest -> InvarRight = atof(argv[9]) / 1000.0f;
            if(Dest -> InvarRight < 0.0f)
            {
                Ret = 0;
                fprintf(stderr, "[Error] Invalid end bank as '%s'.\n", argv[9]);
                break;
            }
            -- CLV;
            
        case 9:
            Dest -> FixedLength = atof(argv[8]) / 1000.0f;
            if(Dest -> FixedLength < 0.0f)
            {
                Ret = 0;
                fprintf(stderr, "[Error] Invalid fixed length as '%s'.\n",
                    argv[8]);
                break;
            }
            -- CLV;
            
        case 8:
            Dest -> LenRequire = atof(argv[7]) / 1000.0f;
            if(Dest -> LenRequire < 0.0f)
            {
                Ret = 0;
                fprintf(stderr, "[Error] Invalid length require as '%s'.\n",
                    argv[7]);
                break;
            }
            Dest -> InvarLeft = atof(argv[6]) / 1000.0f;
            if(Dest -> InvarLeft < 0.0f)
            {
                Ret = 0;
                fprintf(stderr, "[Error] Invalid offset as '%s'.\n", argv[6]);
                break;
            }
            CLV -= 2;
            
        case 6:;
            char* CFlags = argv[5];
            int FlagLen = strlen(CFlags);
            i = 0;
            
            #define IsLetter(x) (((x) >= 'a' && (x) <= 'z') || \
                                 ((x) >= 'A' && (x) <= 'Z'))
            //Parse flags
            while(i < FlagLen)
            {
                float Value = atof(CFlags + i + 1);
                switch(CFlags[i])
                {
                    case 'B':
                        //Avoid floating point error due to extremely small
                        //  number in log scale converison.
                        if(Value < 0.1)
                            Value = 0.1;
                        
                        Dest -> FlagPara.Breathness    = Value;
                        if(Value < 0)
                            fprintf(stderr, "[Warning] Invalid breathness "
                                "parameter.\n");
                    break;
                    case 'g':
                        Dest -> FlagPara.Gender        = Value;
                        if(Value <= 0)
                            fprintf(stderr, "[Warning] Invalid gender parameter"
                                ".\n");
                    break;
                    case 'u':
                        Dest -> FlagPara.CLoudness     = Value;
                        if(Value < 0 || Value > 3)
                            fprintf(stderr, "[Warning] Invalid unvoiced "
                                "consonant loudness parameter.\n");
                    break;
                    case 'c':
                        Dest -> FlagPara.CStretch      = Value;
                    break;
                    case 'o':
                        Dest -> FlagPara.COffset       = Value;
                    break;
                    case 'm':
                        Dest -> FlagPara.SmoothenRate  = Value;
                        if(Value < 0 || Value > 1)
                            fprintf(stderr, "[Warning] Invalid smoothen rate "
                                "parameter.\n");
                    break;
                    case 'r':
                        Dest -> FlagPara.SmoothenRadius = Value;
                        if(Value <= 0)
                            fprintf(stderr, "[Warning] Invalid smoothen radius "
                                "parameter.\n");
                    break;
                    case 'S':
                        if(i + 2 > FlagLen || (CFlags[i + 1] != '1' &&
                           CFlags[i + 1] != '2'))
                        {
                            fprintf(stderr, "[Warning] Invalid segmentation " \
                                "adjustment parameter.\n");
                            break;
                        }
                        Value = atof(CFlags + i + 2);
                        if(CFlags[i + 1] == '1')
                            Dest -> FlagPara.DeltaSeg1 = Value;
                        if(CFlags[i + 1] == '2')
                            Dest -> FlagPara.DeltaSeg2 = Value;
                    break;
                    case 'd':
                        Dest -> FlagPara.DeltaDuration = Value;
                    break;
                    case 'p':
                        Dest -> FlagPara.PhaseSync     = Value;
                        if(Value < 0)
                            fprintf(stderr, "[Warning] Invalid phase "
                                "synchronicity.\n");
                    break;
                    case 'V':
                        Dest -> FlagPara.Verbose       = 1;
                    break;
                    default:
                        fprintf(stderr, "[Warning] Unrecognized flag '%c' with "
                            "parameter %f.\n", CFlags[i], Value);
                }
                
                i ++;
                while(i < FlagLen && ! IsLetter(CFlags[i])) i ++;
            }
            -- CLV;
            
        case 5:
            Dest -> Velocity = atof(argv[4]);
            if(Dest -> Velocity < 0.0f)
            {
                Ret = 0;
                fprintf(stderr, "[Error] Invalid velocity as '%s'.\n", argv[4]);
                break;
            }
            if(EnablePitchConv)
            {
                String_SetChars(& PP, argv[3]);
                PMatch_Float_Float_AddPair(& Dest -> Freq, 0, 
                    Tune_SPNToFreq_Float(& PP));
            }
            String_SetChars(& (Dest -> Input), argv[1]);
            String_SetChars(& (Dest -> Output), argv[2]);
            break;
            
        default:
            PrintUsage(argv[0]);
            Ret = -1;
    };
    RExit:

    RDelete(& PP, & PBD);
    return Ret;
}


#include "type_define.h"
#include <float.h>

float wrap(float value, float from, float to)
{
    // algorithm from http://stackoverflow.com/a/5852628/599884
    float temp = 0;
    if(from > to)
    {
        temp = from;
        from = to;
        to = temp;
        //swap(from, to);
    }
    float cycle = to - from;
    if(cycle == 0)
    {
        return to;
    }
    return value - cycle * floor((value - from) / cycle);
}

float deg_rad(float degrees)
{
    return degrees * DEG_TO_RAD;
}
float rad_deg(float radians)
{
    return radians * RAD_TO_DEG;
}
float clamp(float value, float min, float max)
{
    return value < min ? min : value > max ? max : value;
}
float map(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp)
{

    if (fabs(inputMin - inputMax) < FLT_EPSILON)
    {
        qDebug() << ("ofMath") << "ofMap(): avoiding possible divide by zero, check inputMin and inputMax: " << inputMin << " " << inputMax;
        return outputMin;
    }
    else
    {
        float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);

        if( clamp )
        {
            if(outputMax < outputMin){
                if( outVal < outputMax )outVal = outputMax;
                else if( outVal > outputMin )outVal = outputMin;
            }else{
                if( outVal > outputMax )outVal = outputMax;
                else if( outVal < outputMin )outVal = outputMin;
            }
        }
        return outVal;
    }

}
float lerp(float start, float stop, float amt)
{
    return start + (stop-start) * amt;
}

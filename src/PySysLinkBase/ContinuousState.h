#ifndef PY_SYS_LINK_BASE_CONTINUOUS_STATE
#define PY_SYS_LINK_BASE_CONTINUOUS_STATE

namespace PySysLinkBase
{
    class ContinuousState
    {
    private:
        double value;
    public:
        ContinuousState(double initialValue);
        ContinuousState();
        double GetValue() const;
        void SetValue(double newValue);
    };
}


#endif /* PY_SYS_LINK_BASE_CONTINUOUS_STATE */

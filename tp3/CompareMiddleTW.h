#ifndef COMPAREMIDDLETW_H
#define COMPAREMIDDLETW_H


class CompareMiddleTW
{
    public:
        /** Default constructor */
        CompareMiddleTW();
        /** Default destructor */
        ~CompareMiddleTW();

        bool operator()(const NodeInfo &, const NodeInfo &);
    protected:
    private:
};

#endif // COMPAREMIDDLETW_H

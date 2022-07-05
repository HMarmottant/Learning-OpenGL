#include "ArbitraryPrecisionFloat.h"
namespace apfloat
{
    apfloat::apfloat(std::string initvalue, unsigned int size)//get string and fit it into the given size, first bit is sign
    {
        sign = initvalue.at(0) == '1';
        initvalue.erase(0,1);
        float_segments = std::vector<bint>(size,0); 
        int cnt = 0;
        for(int i = 0;i<float_segments.size();i++)
        {
            
            if(cnt+BINT_SIZE>initvalue.length())
            {
                float_segments.at(i) = bint(strflip(initvalue.substr(cnt,initvalue.length()-cnt)));
                break;
            } 
            float_segments.at(i) = bint(strflip(initvalue.substr(cnt,BINT_SIZE)));
            cnt+=BINT_SIZE;
        }

        
    }

    apfloat::~apfloat()
    {
        
    }

    void fit(apfloat &A,const apfloat &B)//fits the given variable into the space of this one conserving size may lead to loss of precision if A is smaller than B
    {
        A.sign = B.sign;
        for(int i = 0 ;i<A.float_segments.size();i++)
        {
            if(i<B.float_segments.size())
            {
                A.float_segments.at(i) = B.float_segments.at(i);
            }
            else
            {
                A.float_segments.at(i) = 0;
            }
        }

    }

    void apfloat::sizechange(int num)
    {
        float_segments.resize(num,bint(0));
    }

    apfloat operator>>(const apfloat &A,uint shift)
    {
        apfloat result("0",A.float_segments.size());
        for(int i = result.float_segments.size()-1;i>=0;i--)
        {
            if((result.float_segments.size() > (i - (shift/BINT_SIZE))) && (0 <=  (i - (shift/BINT_SIZE))))
            {
                result.float_segments.at(i) |= (A.float_segments.at(i - (shift/BINT_SIZE))<<(shift%BINT_SIZE));
            }
            if((result.float_segments.size() > (i - (shift/BINT_SIZE) - 1)) && (0 <= (i - (shift/BINT_SIZE) - 1)))
            {
                result.float_segments.at(i) |= (A.float_segments.at(i - (shift/BINT_SIZE) - 1)>>(BINT_SIZE - (shift%BINT_SIZE)));
            }
        }
        result.sign = A.sign;
        return result;
    }

    apfloat operator<<(const apfloat &A,uint shift)
    {
        apfloat result("0",A.float_segments.size());
        for(int i = result.float_segments.size()-1;i>=0;i--)
        {
            if((result.float_segments.size() > (i + (shift/BINT_SIZE))) && (0 <=  (i + (shift/BINT_SIZE))))
            {
                result.float_segments.at(i) |= (A.float_segments.at(i + (shift/BINT_SIZE))>>(shift%BINT_SIZE));
            }
            if((result.float_segments.size() > (i + (shift/BINT_SIZE) + 1)) && (0 <= (i + (shift/BINT_SIZE) + 1)))
            {
                result.float_segments.at(i) |= (A.float_segments.at(i + (shift/BINT_SIZE) + 1)<<(BINT_SIZE - (shift%BINT_SIZE)));
            }
        }
        result.sign = A.sign;
        return result;
    }

    apfloat operator+(const apfloat &A,const apfloat &B)
    {
        apfloat result("0",0);
        result = apfloat("0",(A.size()>B.size())? A.size() : B.size());
        bool borrow = 0;//needed to check fo nagative result
        //addition algorithm
        if(!(A.sign^B.sign)) // check for same sign
        {
            bool carry = 0;
            for(int i = result.size()-1;i>=0;i--)
            {
                if(i<A.float_segments.size() && i<B.float_segments.size())
                {
                    result.float_segments.at(i) = addcarry(A.float_segments.at(i),B.float_segments.at(i),&carry);
                }
                else if(i<A.float_segments.size())
                {
                    result.float_segments.at(i) = addcarry(A.float_segments.at(i),bint("0"),&carry);
                }
                else if(i<B.float_segments.size())
                {
                    result.float_segments.at(i) = addcarry(bint("0"),B.float_segments.at(i),&carry);
                }
                //std::cout << carry << std::endl;
            }
            if(A.sign) result.sign = 1;
        }
        else if( A.sign )//substraction algorithm with A negative
        {
            for(int i = result.size()-1;i>=0;i--)
            {
                if(i<A.float_segments.size() && i<B.float_segments.size())
                {
                    result.float_segments.at(i) = subborrow(B.float_segments.at(i),A.float_segments.at(i),&borrow);
                }
                else if(i<A.float_segments.size())
                {
                    result.float_segments.at(i) = subborrow(bint("0"),A.float_segments.at(i),&borrow);
                }
                else if(i<B.float_segments.size())
                {
                    result.float_segments.at(i) = subborrow(B.float_segments.at(i),bint("0"),&borrow);
                }
                //std::cout << borrow << std::endl;
            }
        }
        else if( B.sign )//substraction algorithm with B negative
        {
            for(int i = result.size()-1;i>=0;i--)
            {
                if(i<A.float_segments.size() && i<B.float_segments.size())
                {
                    result.float_segments.at(i) = subborrow(A.float_segments.at(i),B.float_segments.at(i),&borrow);
                }
                else if(i<B.float_segments.size())
                {
                    result.float_segments.at(i) = subborrow(bint("0"),B.float_segments.at(i),&borrow);
                }
                else if(i<A.float_segments.size())
                {
                    result.float_segments.at(i) = subborrow(A.float_segments.at(i),bint("0"),&borrow);
                }
                //std::cout << borrow << std::endl;
            }
        }
        if(borrow)
        {
            bool carry = 1;
            for(int i = result.size()-1;i>=0;i--)
            {
                result.float_segments.at(i) = addcarry(~result.float_segments.at(i),bint("0"),&carry); 
            }
            result.sign = 1;
        }

        return result;
    }

    apfloat operator-(const apfloat &A,const apfloat &B)
    {
        apfloat tmp = B;
        tmp.flipsign();
        return A + tmp; 
    }

    apfloat operator*(const apfloat &A,const apfloat &B)
    {
        int size = (A.size()>B.size())? A.size() : B.size();
        apfloat res("0",size+1);
        apfloat mul("0",size+1);
        fit(mul,B);
        mul.sign = 0;
        mul = (mul >> BINT_SIZE);
        // std::cout << mul << std::endl;
        int shift=0;
        for(int i = 0; i <A.size()*BINT_SIZE;i++)
        {
            shift = BINT_SIZE - i;
            // std::cout << shift << std::endl;
            if(A.float_segments.at(i/BINT_SIZE).test(i%BINT_SIZE))
            {
                if(shift <0)
                {
                    res = res + (mul>>(-shift));
                }
                else
                {
                    res = res + (mul<<shift);
                }
            }
        }
        if(A.sign^B.sign)//see if singns are diferent
        {
            // std::cout << "b" << std::endl;
            res.sign = 1;
        }
        else
        {
            // std::cout << "a" << std::endl;
            res.sign = 0;
        }
        res = res << BINT_SIZE-1;
        return res;
    }

    apfloat operator/(const apfloat &A,const apfloat &B) //long division of A by B
    {
        int size = (A.size()>B.size())? A.size() : B.size();
        apfloat q("0", 2*size );
        apfloat r("0", 2*size);
        fit(r,A);
        r = r>>(size * BINT_SIZE);
        r.sign = 0;
        // std::cout << r << std::endl;
        apfloat tmp("0",q.size());
        // bool resset = 0;
        // std::cout << buff << std::endl;
        for(int i = 0; i <2*size*BINT_SIZE;i++)
        {
            // std::cout << "test" << std::endl;
            r = r<<1;
            tmp = (B.sign)? r+B : r-B;
            if(tmp.sign == r.sign)
            {
                // std::cout << i << std::endl;
                r = tmp;
                q.float_segments.at(i / BINT_SIZE).flip(i%BINT_SIZE);
            }
        }
        q = q << ((size-1) * BINT_SIZE);
        q.sizechange(size);
        if(A.sign^B.sign)//see if singns are diferent
        {
            r.sign = 1;
            q.sign = 1;
        }
        else
        {
            r.sign = 0;
            q.sign = 0;
        }
        // std::cout << q << std::endl;
        return q;
    }

    apfloat apfloat::extendedRemainderDiv(apfloat B)
    {
        int size = B.size();
        apfloat q("0", 2*size );
        apfloat r("0", 2*size);


        //find last non zero bit
        // int lastbit = 0;
        // for(int i = 0; i <B.size();i++)
        // {
        //     for(int off = 0;off< BINT_SIZE;off++)
        //     {
        //         if(B.float_segments.at(i).test(off))
        //         {
        //             lastbit = i * BINT_SIZE + off;
        //         }
        //     }
            
        // }
        // std::cout << size << std::endl;

        fit(r,*this);
        r = r>>(size * BINT_SIZE);
        r.sign = 0;
        // std::cout << r << std::endl;
        apfloat tmp("0",q.size());
        // bool resset = 0;
        
        for(int i = 0; i <  size*BINT_SIZE ;i++)
        {
            r = r<<1;
            // std::cout << "test" << std::endl;
            tmp = (B.sign)? r+B : r-B;
            // std::cout << i << std::endl;
            // std::cout << q << std::endl;
            // std::cout << r << std::endl;
            // std::cout << tmp << std::endl;
            if(tmp.sign == r.sign)
            {
                r = tmp;
                q.float_segments.at(i / BINT_SIZE).flip(i%BINT_SIZE);
            }
        }

        // std::cout << q << std::endl;
        // std::cout << r << std::endl;
        
        q = q << ((size-1) * BINT_SIZE);
        q.sizechange(size);

        // r.sizechange(size);
        if((*this).sign^B.sign)//see if singns are diferent
        {
            r.sign = 1;
            q.sign = 1;
        }
        else
        {
            r.sign = 0;
            q.sign = 0;
        }

        return q;
    }


    apfloat apfloat::extendedRemainderDivRest(apfloat B)
    {
        int size = B.size();
        apfloat q("0", 2*size );
        apfloat r("0", 2*size);


        //find last non zero bit
        // int lastbit = 0;
        // for(int i = 0; i <B.size();i++)
        // {
        //     for(int off = 0;off< BINT_SIZE;off++)
        //     {
        //         if(B.float_segments.at(i).test(off))
        //         {
        //             lastbit = i * BINT_SIZE + off;
        //         }
        //     }
            
        // }
        // std::cout << size << std::endl;

        fit(r,*this);
        r = r>>(size * BINT_SIZE);
        r.sign = 0;
        // std::cout << r << std::endl;
        apfloat tmp("0",q.size());
        // bool resset = 0;
        
        for(int i = 0; i <  size*BINT_SIZE ;i++)
        {
            r = r<<1;
            // std::cout << "test" << std::endl;
            tmp = (B.sign)? r+B : r-B;
            // std::cout << i << std::endl;
            // std::cout << q << std::endl;
            // std::cout << r << std::endl;
            // std::cout << tmp << std::endl;
            if(tmp.sign == r.sign)
            {
                r = tmp;
                q.float_segments.at(i / BINT_SIZE).flip(i%BINT_SIZE);
            }
        }

        // std::cout << q << std::endl;
        // std::cout << r << std::endl;
        
        // q = q << ((size-1) * BINT_SIZE);
        // q.sizechange(size);

        r.sizechange(size);
        if((*this).sign^B.sign)//see if singns are diferent
        {
            r.sign = 1;
            q.sign = 1;
        }
        else
        {
            r.sign = 0;
            q.sign = 0;
        }

        return r;
    }

  
    bint addcarry(bint a, bint b, bool *carry)//add bint a and b with carry
    {
        bint res = 0;
        if(*carry)
        {
            *carry = 0;
            bint carryadd("10000000000000000000000000000000");
            while(carryadd.any())
            {
                res = a&carryadd;
                a^=carryadd;
                if(res.test(0)) *carry = 1;
                carryadd = res>>1;
            }
        }
        res = 0;
        while(b.any())
        {
            res = a&b;
            a^=b;
            if(res.test(0)) *carry = 1;
            b = res>>1;
        }
        return a;
    }

    bint subborrow(bint a, bint b, bool *borrow)//substract b from a with borrow
    {
        bint bor = 0;
        if(*borrow)
        {
            *borrow = 0;
            bint borrowsub("10000000000000000000000000000000");
            while(borrowsub.any())
            {
                bor = (~a)&borrowsub;
                a^=borrowsub;
                if(bor.test(0)) *borrow = 1;
                borrowsub = bor>>1;
            }
        }
        bor = 0;
        while(b.any())
        {
            bor = (~a)&b;
            a^=b;
            if(bor.test(0)) *borrow = 1;
            b = bor>>1;
        }
        return a;
    }



    std::ostream& operator<<(std::ostream& os, const apfloat &A)
    {
        os <<"Sign: "<< A.sign << " Bite values: ";
        for(int i = 0 ;i<A.float_segments.size();i++)
        {
            os << i <<"# "<< binttostr(A.float_segments[i]) <<" ";
        }
        return os;
    }

    std::string strflip(std::string str)
    {
        std::string tmp = str;
        for(int i = 0;i<str.length();i++)
        {
            tmp[i] = str[str.length()-1-i];
        }
        return tmp;
    }

    std::string binttostr(bint a)
    {
        std::string str = "";
        for(int j = 0;j<BINT_SIZE;j++) str += a.test(j)?"1":"0";
        return str;
    }

    std::string apfloat::tobasestring(apfloat base)//returns number in base B as a string rounded down to a positive integer
    {
        int size = (*this).size();
        std::string strint = "";
        std::string strdec = "";
        int digit = 0;

        apfloat correctbase = base;
        correctbase.sizechange(1);
        correctbase.sign = 0;
        if(correctbase.null()) return "";

        apfloat temp = *this;
        temp.sizechange(1);
        // std::cout << temp << std::endl;
        while(!temp.null())        
        {
            digit =  temp.extendedRemainderDivRest(correctbase).getintegerpart();
            if(digit < 10)
            {
                strint += ('0' + digit);
            }
            else
            {
                strint += ('A' + digit - 9);
            }
            
            temp = temp.extendedRemainderDiv(correctbase);
        }

        temp = *this;
        temp = (temp << BINT_SIZE) >> BINT_SIZE;
        // std::cout <<"tamp"<< temp << std::endl;

        while(!temp.null())        
        {
            // std::cout <<"temp" << temp << std::endl;

            temp = temp * correctbase;

            // std::cout <<"temp10" << temp << std::endl;
            digit = temp.getintegerpart();

            if(digit < 10)
            {
                strdec += ('0' + digit);
            }
            else
            {
                strdec += ('A' + digit - 9);
            }

            temp = (temp << BINT_SIZE) >> BINT_SIZE;
        }

        return strflip(strint) + "," + strdec;
    }

    bool apfloat::null()
    {
        for(int i = 0 ; i < (*this).size();i++)
        {
            if((*this).float_segments.at(i).any())
                return 0;
        }
        return 1;
    }

    int apfloat::getintegerpart()
    {
        return stoi(strflip((*this).float_segments.at(0).to_string()),0,2);
    }

    extendedFloatArray apfloat::getasuintarray()
    {
        uint* array = (uint*)malloc(sizeof(uint) * (*this).size());
        uint* temp = array;
        for (int i = 0; i < (*this).size(); i++)
        {
            *temp = stoi(strflip((*this).float_segments.at(i).to_string()), 0, 2);
            temp++;
        }

        return extendedFloatArray{ (uint)(*this).size(),array };
    }
}
class processor_status_register {    
    private:
        unsigned int cwp;
        unsigned int et;
        unsigned int ps;
        unsigned int s;
        unsigned int pil;
        unsigned int ef;
        unsigned int ec;
        unsigned int reserved;

        unsigned int c;
        unsigned int v;
        unsigned int z;
        unsigned int n;

        unsigned int ver;
        unsigned int impl;

    public:
        int getCwp()
        {
            return this->cwp;
        }

        void setCwp(int cwp)
        {
            this->cwp = cwp;
        }

        int getEt()
        {
            return this->et;
        }

        void setEt(int et)
        {
            this->et = et;
        }

        int getPs()
        {
            return this->ps;
        }

        void setPs(int ps)
        {
            this->ps = ps;
        }

        int getS()
        {
            return this->s;
        }

        void setS(int s)
        {
            this->s = s;
        }

        int getPil()
        {
            return this->pil;
        }

        void setPil(int pil)
        {
            this->pil = pil;
        }

        int getEf()
        {
            return this->ef;
        }

        void setEf(int ef)
        {
            this->ef = ef;
        }

        int getEc()
        {
            return this->ec;
        }

        void setEc(int ec)
        {
            this->ec = ec;
        }

        int getReserved()
        {
            return this->reserved;
        }

        void setReserved(int reserved)
        {
            this->reserved = reserved;
        }

        int getC()
        {
            return this->c;
        }

        void setC(int c)
        {
            this->c = c;
        }

        int getV()
        {
            return this->v;
        }

        void setV(int v)
        {
            this->v = v;
        }

        int getZ()
        {
            return this->z;
        }

        void setZ(int z)
        {
            this->z = z;
        }

        int getN()
        {
            return this->n;
        }

        void setN(int n)
        {
            this->n = n;
        }

        int getVer()
        {
            return this->ver;
        }

        void setVer(int ver)
        {
            this->ver = ver;
        }

        int getImpl()
        {
            return this->impl;
        }

        void setImpl(int impl)
        {
            this->impl = impl;
        }

}__attribute__ ((__packed__));

class floating_point_state_register{

    private:
        unsigned int cexc;
        unsigned int aexc;
        unsigned int fcc;
        unsigned int ulow;
        unsigned int qne;
        unsigned int ftt;
        unsigned int ver;
        unsigned int res;
        unsigned int ns;
        unsigned int tem;
        unsigned int uhigh;
        unsigned int rd;

    public:
        int getCexc()
        {
            return this->cexc;
        }

        void setCexc(int cexc)
        {
            this->cexc = cexc;
        }

        int getAexc()
        {
            return this->aexc;
        }

        void setAexc(int aexc)
        {
            this->aexc = aexc;
        }

        int getFcc()
        {
            return this->fcc;
        }

        void setFcc(int fcc)
        {
            this->fcc = fcc;
        }

        int getUlow()
        {
            return this->ulow;
        }

        void setUlow(int ulow)
        {
            this->ulow = ulow;
        }

        int getQne()
        {
            return this->qne;
        }

        void setQne(int qne)
        {
            this->qne = qne;
        }

        int getFtt()
        {
            return this->ftt;
        }

        void setFtt(int ftt)
        {
            this->ftt = ftt;
        }

        int getVer()
        {
            return this->ver;
        }

        void setVer(int ver)
        {
            this->ver = ver;
        }

        int getRes()
        {
            return this->res;
        }

        void setRes(int res)
        {
            this->res = res;
        }

        int getNs()
        {
            return this->ns;
        }

        void setNs(int ns)
        {
            this->ns = ns;
        }

        int getTem()
        {
            return this->tem;
        }

        void setTem(int tem)
        {
            this->tem = tem;
        }

        int getUhigh()
        {
            return this->uhigh;
        }

        void setUhigh(int uhigh)
        {
            this->uhigh = uhigh;
        }

        int getRd()
        {
            return this->rd;
        }

        void setRd(int rd)
        {
            this->rd = rd;
        }

}__attribute__ ((__packed__));

/***********************************************************************\
|                                                                       |
| BitStringOperator.h |
|                                                                       |
| This file is part of MicroGP v3 (ugp3)                                |
| http://ugp3.sourceforge.net/                                          |
|                                                                       |
| Copyright (c) 2002-2016 Giovanni Squillero                            |
|                                                                       |
|-----------------------------------------------------------------------|
|                                                                       |
| This program is free software; you can redistribute it and/or modify  |
| it under the terms of the GNU General Public License as published by  |
| the Free Software Foundation, either version 3 of the License, or (at |
| your option) any later version.                                       |
|                                                                       |
| This program is distributed in the hope that it will be useful, but   |
| WITHOUT ANY WARRANTY; without even the implied warranty of            |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      |
| General Public License for more details                               |
|                                                                       |
|***********************************************************************'
| $Revision: 644 $
| $Date: 2015-02-23 14:50:30 +0100 (Mon, 23 Feb 2015) $
\***********************************************************************/

/**
 * @file BitStringOperator.h
 * Blah
 */

#ifndef HEADER_UGP3_CORE_BITSTRINGOPERATOR
#define HEADER_UGP3_CORE_BITSTRINGOPERATOR

#include <Mutation.h>

namespace ugp3 {
namespace core {

class BitStringOperator : public MutationOperator
{
protected:
    /**
     * Return the most encountered bit in
     * - global == true → the whole bit string,
     * - global == false → a neighbourhood of the position
     * or the bit at the given position in case of a tie.
     */
    bool majority(const std::string& bits, size_t position, bool global) const;
    
    virtual void mutateBitString(std::string& string) const = 0;
    
    /* 
     * For tests according to Fialho's paper,
     * some operators must do the mutation exactly once.
     */
    bool useSigma = true;
    
public:
    virtual void generate(const std::vector<Individual*>& parents,
                          std::vector<Individual*>& outChildren,
                          IndividualPopulation& population) const;
                          
    virtual Operator::Category getCategory() { return DEFAULT_OFF; }
    
    virtual bool hasParameters() const { return false; }
    virtual void readXml(const xml::Element& element) {};
    virtual void writeXml(std::ostream& output) const {};
};

class BitStringResetOneMutation : public BitStringOperator
{
protected:
    virtual void mutateBitString(std::string& string) const;
    
public:
    virtual const std::string getAcronym() const { return "BS.RO.M"; }
    virtual const std::string getDescription() const { return "Selects one bit array parameter and zeroes one bit. Repeat the process when sigma is high"; }
    virtual const std::string getName() const { return "bitStringResetOneMutation"; }
};

class BitStringFlipOneMutation : public BitStringOperator
{
protected:
    virtual void mutateBitString(std::string& string) const;
    
public:
    virtual const std::string getAcronym() const { return "BS.FO.M"; }
    virtual const std::string getDescription() const { return "Selects one bit array parameter and flips one bit. Repeat the process when sigma is high"; }
    virtual const std::string getName() const { return "bitStringFlipOneMutation"; }
};

class BitStringSetOneMutation : public BitStringOperator
{
protected:
    virtual void mutateBitString(std::string& string) const;
    
public:
    virtual const std::string getAcronym() const { return "BS.SO.M"; }
    virtual const std::string getDescription() const { return "Selects one bit array parameter and sets one bit to one. Repeat the process when sigma is high"; }
    virtual const std::string getName() const { return "bitStringSetOneMutation"; }
};

class BitStringLocalMajorityMutation : public BitStringOperator
{
protected:
    virtual void mutateBitString(std::string& string) const;
    
public:
    virtual const std::string getAcronym() const { return "BS.LMa.M"; }
    virtual const std::string getDescription() const { return "Selects one bit array parameter and sets one bit to the majority of a neighbourhood of this bit. Repeat the process when sigma is high"; }
    virtual const std::string getName() const { return "bitStringLocalMajorityMutation"; }
};

class BitStringLocalMinorityMutation : public BitStringOperator
{
protected:
    virtual void mutateBitString(std::string& string) const;
    
public:
    virtual const std::string getAcronym() const { return "BS.LMi.M"; }
    virtual const std::string getDescription() const { return "Selects one bit array parameter and sets one bit to the minority of a neighbourhood of this bit. Repeat the process when sigma is high"; }
    virtual const std::string getName() const { return "bitStringLocalMinorityMutation"; }
};

class BitStringGlobalMajorityMutation : public BitStringOperator
{
protected:
    virtual void mutateBitString(std::string& string) const;
    
public:
    virtual const std::string getAcronym() const { return "BS.GMa.M"; }
    virtual const std::string getDescription() const { return "Selects one bit array parameter and sets one bit to the majority of the whole bitarray. Repeat the process when sigma is high"; }
    virtual const std::string getName() const { return "bitStringGlobalMajorityMutation"; }
};

class BitStringGlobalMinorityMutation : public BitStringOperator
{
protected:
    virtual void mutateBitString(std::string& string) const;
    
public:
    virtual const std::string getAcronym() const { return "BS.GMi.M"; }
    virtual const std::string getDescription() const { return "Selects one bit array parameter and sets one bit to the minority of the whole bitarray. Repeat the process when sigma is high"; }
    virtual const std::string getName() const { return "bitStringGlobalMinorityMutation"; }
};

/*
 * Operators described in Álvaro Fialho, Luis Da Costa, Marc Schoenauer,
 * Michèle Sebag. Dynamic Multi-Armed Bandits and Extreme Value-based
 * Rewards for Adaptive Operator Selection in Evolutionary Algorithms.
 * Learning and Intelligent Optimization (LION 3), Jan 2009, Trento,
 * Italy. <inria-00377401v1>
 */
class BitStringFlipUniformMutation : public BitStringOperator
{
protected:
    virtual void mutateBitString(std::string& string) const;
    
public:
    BitStringFlipUniformMutation() { useSigma = false; }
    
    virtual const std::string getAcronym() const { return "BS.FU.M"; }
    virtual const std::string getDescription() const { return "Selects one bit array parameter and flips each bit with probability 1/length."; }
    virtual const std::string getName() const { return "bitStringFlipUniformMutation"; }
};

class BitStringFlip1Mutation : public BitStringOperator
{
protected:
    virtual void mutateBitString(std::string& string) const;
    
public:
    BitStringFlip1Mutation() { useSigma = false; }
    
    virtual const std::string getAcronym() const { return "BS.F1.M"; }
    virtual const std::string getDescription() const { return "Selects one bit array parameter and flips exactly 1 bit."; }
    virtual const std::string getName() const { return "bitStringFlip1Mutation"; }
};

class BitStringFlip3Mutation : public BitStringOperator
{
protected:
    virtual void mutateBitString(std::string& string) const;
    
public:
    BitStringFlip3Mutation() { useSigma = false; }
    
    virtual const std::string getAcronym() const { return "BS.F3.M"; }
    virtual const std::string getDescription() const { return "Selects one bit array parameter and flips exactly 3 bits."; }
    virtual const std::string getName() const { return "bitStringFlip3Mutation"; }
};

class BitStringFlip5Mutation : public BitStringOperator
{
protected:
    virtual void mutateBitString(std::string& string) const;
    
public:
    BitStringFlip5Mutation() { useSigma = false; }
    
    virtual const std::string getAcronym() const { return "BS.F5.M"; }
    virtual const std::string getDescription() const { return "Selects one bit array parameter and flips exactly 5 bits."; }
    virtual const std::string getName() const { return "bitStringFlip5Mutation"; }
};

}
}

#endif // HEADER_UGP3_CORE_BITSTRINGOPERATOR

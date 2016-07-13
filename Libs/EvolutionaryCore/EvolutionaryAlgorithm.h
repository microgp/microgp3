/***********************************************************************\
|                                                                       |
| EvolutionaryAlgorithm.h                                               |
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
| $Revision: 656 $
| $Date: 2015-04-15 14:16:06 +0200 (Wed, 15 Apr 2015) $
\***********************************************************************/

#ifndef HEADER_UGP3_CORE_EVOLUTIONARYALGORITHM
#define HEADER_UGP3_CORE_EVOLUTIONARYALGORITHM

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include "XMLIFace.h"
#include <File.h>

#include "Evaluator.h"
#include "Population.h"
#include "Constraints.h"
#include "IMigrator.h"
#include "Operator.h"

namespace ugp3
{

    namespace core
    {

        class EvolutionaryAlgorithm : public xml::XMLIFace
        {
        private:
            std::string         outputPathName;
            bool                overwriteOutput;
            std::string         statisticsPathName;
	    std::ofstream	statisticsStream;
            unsigned int        algorithmStep;
            IMigrator*          migrator;
    
            chrono::seconds m_elapsedTimeThisRun;
            chrono::seconds m_elapsedTimePreviousRuns;
            chrono::time_point<chrono::steady_clock> m_startTime;

        protected:
            std::vector<Population*> populations;

            EvolutionaryAlgorithm(const EvolutionaryAlgorithm& ea);
            void parsePopulations(const xml::Element& element);
            void parseOperators(const xml::Element& element);

            /** Performs a step of the evolutionary algorithm.*/
            bool step();

        protected:
            static const std::string XML_ATTRIBUTE_STATISTICSFILE;
            static const std::string XML_ATTRIBUTE_DATETIME;
            static const std::string XML_NAME;
            static const std::string XML_NAMESPACE;
            static const std::string XML_CHILD_ELEMENT_POPULATIONS;
            static const std::string XML_ATTRIBUTE_STEP;
            static const std::string XML_ATTRIBUTE_ELAPSEDTIME_PREVIOUSRUNS;
            static const std::string XML_ATTRIBUTE_ELAPSEDTIME_CURRENTRUN;
            static const std::string XML_ATTRIBUTE_OUTPUTFILE;
            static const std::string XML_ATTRIBUTE_OVERWRITEOUTPUT;
            static const std::string XML_ATTRIBUTE_RANDOMSTATE;
            static const std::string XML_ATTRIBUTE_RANDOMTYPE;
            static const std::string XML_STEP;
            static const std::string XML_STEPS;
		
	    static const std::string XML_VERSION;

        public: // constructors and destructors
            EvolutionaryAlgorithm();
            ~EvolutionaryAlgorithm();

        public: // methods
            void fromFile(const std::string& xmlFileName, bool discardFitness);
            void save(const std::string& xmlFile) const;
            void saveDumpState() const;

            /** It starts the evolutionary algorithm.
                It's a loop containing a call to the method "EvolutionaryAlgorithm::step".*/
            void run();
            void addPopulation(std::unique_ptr<Population> population);
            std::unique_ptr<Population> removePopulation(unsigned int index);
	    // methods added to manage the statistics from outside
	    void openStatisticsStream(void);
	    void writeStatisticsStream(void);

        public: // getters
            const std::string&  getDumpPath() const;
            unsigned int        getMaximumSteadyStateGenerations() const;
            unsigned int        getStep() const;
            Population&         getPopulation(unsigned int index) const;
            unsigned int        getPopulationCount() const;
            
            chrono::time_point<chrono::steady_clock> getStartTime() const 
	    {
                return m_startTime;
            }
            chrono::seconds getElapsedTime(void) const 
	    {
                return m_elapsedTimeThisRun + m_elapsedTimePreviousRuns;
            }
            void setElapsedTime(chrono::seconds elapsed) 
	    {
                m_elapsedTimeThisRun = elapsed;
            }

        public: // setters
            void                setOutputPathName(const std::string& value);
            void                setOverwriteOutput(bool value);
            void                setStatisticsPathName(const std::string& value);
            void                setMigrator(IMigrator* value);

        public: // Xml interface
            virtual void writeXml(std::ostream& output) const;
            virtual void readXml(const xml::Element& element);
            virtual const std::string& getXmlName() const;
        };


        inline unsigned int EvolutionaryAlgorithm::getStep() const
        {
            return this->algorithmStep;
        }

        inline void EvolutionaryAlgorithm::setMigrator(IMigrator* value)
        {
            this->migrator = value;
        }

        inline unsigned int EvolutionaryAlgorithm::getPopulationCount() const
        {
            return(unsigned int)this->populations.size();
        }

        inline const std::string& EvolutionaryAlgorithm::getXmlName() const
        {
            return XML_NAME;
        }

        inline const std::string& EvolutionaryAlgorithm::getDumpPath() const
        {
            return this->outputPathName;
        }

        inline void EvolutionaryAlgorithm::setOutputPathName(const std::string& value)
        {
            this->outputPathName = value;
        }

        inline void EvolutionaryAlgorithm::setOverwriteOutput(bool value)
        {
            this->overwriteOutput = value;
        }

        inline void EvolutionaryAlgorithm::setStatisticsPathName(const std::string& value)
        {
            string statFileName;
            if (File::exists(value)) 
	    {
                statFileName = File::appendTimeToName(value);
                LOG_WARNING << "File \"" << value << "\" already exists. Dumping statistics to \"" << statFileName << "\"" << ends;
            } 
            else
            {
                statFileName = value;
            }
            this->statisticsPathName = statFileName;
        }

        inline std::unique_ptr<Population> EvolutionaryAlgorithm::removePopulation(unsigned int index)
        {
            std::unique_ptr<Population> population (this->populations[index]);
            this->populations.erase(this->populations.begin() + index);

            return population;
        }

    }

}

#endif

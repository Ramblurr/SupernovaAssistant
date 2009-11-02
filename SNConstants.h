#ifndef SNCONSTANTS_H
#define SNCONSTANTS_H

#include <QString>
namespace SN
{
    class Category
    {
        public:
            Category(){}

            static QString ShipComponent; // main category
            // sub categories
            static QString Bridge;
            static QString Armor;
            static QString Drone;
            static QString Engine;
            static QString JumpDrive;
            static QString DiplomaticFacility;
            static QString ScienceLab;
            static QString Shield;
            static QString DefensiveSystem;
            static QString FighterBay;
            static QString Sensor;
            static QString Weapon;

            static QString Technology;  // main category
            // sub categories
            static QString HorizonTechnology;
            static QString GroundTechnology;

            static QString Resource; // main category
            // sub categories
            static QString InitialResource;
            static QString ImprovedResource;
            static QString AdvancedResource;

            static QString MDD; // main category

            static QString Unknown; // main category
    };
}

#endif // SNCONSTANTS_H

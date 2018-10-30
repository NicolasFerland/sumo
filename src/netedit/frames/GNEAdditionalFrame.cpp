/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAdditionalFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
// The Widget for add additional elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEAdditionalHandler.h>
#include <netedit/additionals/GNECalibrator.h>
#include <netedit/additionals/GNEClosingLaneReroute.h>
#include <netedit/additionals/GNEClosingReroute.h>
#include <netedit/additionals/GNERerouter.h>
#include <netedit/additionals/GNERerouterInterval.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/additionals/GNEDestProbReroute.h>
#include <netedit/additionals/GNERerouter.h>
#include <netedit/additionals/GNEVariableSpeedSign.h>
#include <netedit/additionals/GNEVariableSpeedSignStep.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNENet.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/GNEUndoList.h>

#include "GNEAdditionalFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAdditionalFrame::AdditionalAttributeSingle) AdditionalAttributeSingleMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_TEXT,     GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,     GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetBooleanAttribute),
};

FXDEFMAP(GNEAdditionalFrame::AdditionalAttributes) AdditionalAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEAdditionalFrame::AdditionalAttributes::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::SelectorLaneParents) ConsecutiveLaneSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_STOPSELECTION,  GNEAdditionalFrame::SelectorLaneParents::onCmdStopSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_ABORTSELECTION, GNEAdditionalFrame::SelectorLaneParents::onCmdAbortSelection),
};

FXDEFMAP(GNEAdditionalFrame::SelectorEdgeChilds) SelectorParentEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTED,        GNEAdditionalFrame::SelectorEdgeChilds::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARSELECTION,     GNEAdditionalFrame::SelectorEdgeChilds::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTSELECTION,    GNEAdditionalFrame::SelectorEdgeChilds::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCH,             GNEAdditionalFrame::SelectorEdgeChilds::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECT,             GNEAdditionalFrame::SelectorEdgeChilds::onCmdSelectEdge),
};

FXDEFMAP(GNEAdditionalFrame::SelectorLaneChilds) SelectorParentLanesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTED,        GNEAdditionalFrame::SelectorLaneChilds::onCmdUseSelectedLanes),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARSELECTION,     GNEAdditionalFrame::SelectorLaneChilds::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTSELECTION,    GNEAdditionalFrame::SelectorLaneChilds::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCH,             GNEAdditionalFrame::SelectorLaneChilds::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECT,             GNEAdditionalFrame::SelectorLaneChilds::onCmdSelectLane),
};

// Object implementation
FXIMPLEMENT(GNEAdditionalFrame::AdditionalAttributeSingle,  FXHorizontalFrame,  AdditionalAttributeSingleMap,   ARRAYNUMBER(AdditionalAttributeSingleMap))
FXIMPLEMENT(GNEAdditionalFrame::AdditionalAttributes,       FXGroupBox,         AdditionalAttributesMap,        ARRAYNUMBER(AdditionalAttributesMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorLaneParents,        FXGroupBox,         ConsecutiveLaneSelectorMap,     ARRAYNUMBER(ConsecutiveLaneSelectorMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorEdgeChilds,         FXGroupBox,         SelectorParentEdgesMap,         ARRAYNUMBER(SelectorParentEdgesMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorLaneChilds,         FXGroupBox,         SelectorParentLanesMap,         ARRAYNUMBER(SelectorParentLanesMap))


// ---------------------------------------------------------------------------
// GNEAdditionalFrame::AdditionalAttributeSingle - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::AdditionalAttributeSingle::AdditionalAttributeSingle(AdditionalAttributes* additionalAttributesParent) :
    FXHorizontalFrame(additionalAttributesParent, GUIDesignAuxiliarHorizontalFrame),
    myAdditionalAttributesParent(additionalAttributesParent),
    myAdditionalAttr(SUMO_ATTR_NOTHING) {
    // Create visual elements
    myLabel = new FXLabel(this, "name", nullptr, GUIDesignLabelAttribute);
    myTextFieldInt = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextFieldInt);
    myTextFieldReal = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextFieldReal);
    myTextFieldStrings = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextField);
    myBoolCheckButton = new FXCheckButton(this, "Disabled", this, MID_GNE_SET_ATTRIBUTE_BOOL, GUIDesignCheckButtonAttribute);
    // Hide elements
    hideAdditionalAttribute();
}


GNEAdditionalFrame::AdditionalAttributeSingle::~AdditionalAttributeSingle() {}


void
GNEAdditionalFrame::AdditionalAttributeSingle::showAdditionalAttribute(SumoXMLAttr additionalAttr, std::string value) {
    myAdditionalAttr = additionalAttr;
    myInvalidValue = "";
    myLabel->setText(toString(myAdditionalAttr).c_str());
    myLabel->show();
    // Retrieve attribute properties
    const auto& attributeProperties = GNEAttributeCarrier::getTagProperties(myAdditionalAttributesParent->myAdditionalFrameParent->myItemSelector->getCurrentTypeTag()).getAttribute(additionalAttr);
    if (attributeProperties.isInt()) {
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->setText(toString(value).c_str());
        myTextFieldInt->show();
    } else if (attributeProperties.isFloat()) {
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->setText(toString(value).c_str());
        myTextFieldReal->show();
    } else if (attributeProperties.isBool()) {
        if (GNEAttributeCarrier::parse<bool>(value)) {
            myBoolCheckButton->setCheck(true);
            myBoolCheckButton->setText("true");
        } else {
            myBoolCheckButton->setCheck(false);
            myBoolCheckButton->setText("false");
        }
        myBoolCheckButton->show();
    } else {
        myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myTextFieldStrings->setText(value.c_str());
        myTextFieldStrings->show();
    }
    show();
}


void
GNEAdditionalFrame::AdditionalAttributeSingle::hideAdditionalAttribute() {
    myAdditionalAttr = SUMO_ATTR_NOTHING;
    myLabel->hide();
    myTextFieldInt->hide();
    myTextFieldReal->hide();
    myTextFieldStrings->hide();
    myBoolCheckButton->hide();
    hide();
}


SumoXMLAttr
GNEAdditionalFrame::AdditionalAttributeSingle::getAttr() const {
    return myAdditionalAttr;
}


std::string
GNEAdditionalFrame::AdditionalAttributeSingle::getValue() const {
    // obtain attribute property (only for improve code legibility)
    const auto& attrValue = GNEAttributeCarrier::getTagProperties(myAdditionalAttributesParent->myAdditionalFrameParent->myItemSelector->getCurrentTypeTag()).getAttribute(myAdditionalAttr);
    // return value depending of attribute type
    if (attrValue.isBool()) {
        return (myBoolCheckButton->getCheck() == 1) ? "true" : "false";
    } else if (attrValue.isInt()) {
        return myTextFieldInt->getText().text();
    } else if (attrValue.isFloat() || attrValue.isTime()) {
        return myTextFieldReal->getText().text();
    } else {
        return myTextFieldStrings->getText().text();
    }
}


const std::string&
GNEAdditionalFrame::AdditionalAttributeSingle::isAttributeValid() const {
    return myInvalidValue;
}


long
GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // We assume that current value is valid
    myInvalidValue = "";
    // get attribute Values (only for improve efficiency)
    const auto& attrValues = GNEAttributeCarrier::getTagProperties(myAdditionalAttributesParent->myAdditionalFrameParent->myItemSelector->getCurrentTypeTag()).getAttribute(myAdditionalAttr);
    // Check if format of current value of myTextField is correct
    if (attrValues.isInt()) {
        if (GNEAttributeCarrier::canParse<int>(myTextFieldInt->getText().text())) {
            // convert string to int
            int intValue = GNEAttributeCarrier::parse<int>(myTextFieldInt->getText().text());
            // Check if int value must be positive
            if (attrValues.isPositive() && (intValue < 0)) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + toString(myAdditionalAttr) + "' doesn't have a valid 'int' format";
        }
    } else if (attrValues.isTime()) {
        // time attributes work as positive doubles
        if (GNEAttributeCarrier::canParse<double>(myTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myTextFieldReal->getText().text());
            // Check if parsed value is negative
            if (doubleValue < 0) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + toString(myAdditionalAttr) + "' doesn't have a valid 'time' format";
        }
    } else if (attrValues.isFloat()) {
        if (GNEAttributeCarrier::canParse<double>(myTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myTextFieldReal->getText().text());
            // Check if double value must be positive
            if (attrValues.isPositive() && (doubleValue < 0)) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' cannot be negative";
                // check if double value is a probability
            } else if (attrValues.isProbability() && ((doubleValue < 0) || doubleValue > 1)) {
                myInvalidValue = "'" + toString(myAdditionalAttr) + "' takes only values between 0 and 1";
            }
        } else {
            myInvalidValue = "'" + toString(myAdditionalAttr) + "' doesn't have a valid 'float' format";
        }
    } else if (attrValues.isFilename()) {
        // check if filename format is valid
        if (SUMOXMLDefinitions::isValidFilename(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "input contains invalid characters for a filename";
        }
    } else if (attrValues.isVClass() && attrValues.isList()) {
        // check if VClasses are valid
        if (canParseVehicleClasses(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "list of VClass isn't valid";
        }
    } else if (myAdditionalAttr == SUMO_ATTR_ROUTEPROBE) {
        // check if filename format is valid
        if (!SUMOXMLDefinitions::isValidNetID(myTextFieldStrings->getText().text())) {
            myInvalidValue = "RouteProbe ID contains invalid characters";
        }
    } else if (myAdditionalAttr == SUMO_ATTR_NAME) {
        // check if filename format is valid
        if (SUMOXMLDefinitions::isValidAttribute(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "Name contains invalid characters";
        }
    } else if (myAdditionalAttr == SUMO_ATTR_VTYPES) {
        // check if filename format is valid
        if (SUMOXMLDefinitions::isValidListOfTypeID(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "Ids contains invalid characters for vehicle type ids";
        }
    }
    // change color of text field depending of myCurrentValueValid
    if (myInvalidValue.size() == 0) {
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->killFocus();
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->killFocus();
        myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myTextFieldStrings->killFocus();
    } else {
        // IF value of TextField isn't valid, change their color to Red
        myTextFieldInt->setTextColor(FXRGB(255, 0, 0));
        myTextFieldReal->setTextColor(FXRGB(255, 0, 0));
        myTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
    }
    // Update aditional frame
    update();
    return 1;
}


long
GNEAdditionalFrame::AdditionalAttributeSingle::onCmdSetBooleanAttribute(FXObject*, FXSelector, void*) {
    if (myBoolCheckButton->getCheck()) {
        myBoolCheckButton->setText("true");
    } else {
        myBoolCheckButton->setText("false");
    }
    return 0;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::AdditionalAttributes - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::AdditionalAttributes::AdditionalAttributes(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // Create single parameters
    for (int i = 0; i < GNEAttributeCarrier::getHigherNumberOfAttributes(); i++) {
        myVectorOfsingleAdditionalParameter.push_back(new AdditionalAttributeSingle(this));
    }
    // Create help button
    new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEAdditionalFrame::AdditionalAttributes::~AdditionalAttributes() {}


void
GNEAdditionalFrame::AdditionalAttributes::clearAttributes() {
    // Hide all fields
    for (int i = 0; i < (int)myVectorOfsingleAdditionalParameter.size(); i++) {
        myVectorOfsingleAdditionalParameter.at(i)->hideAdditionalAttribute();
    }
}


void
GNEAdditionalFrame::AdditionalAttributes::addAttribute(SumoXMLAttr AdditionalAttributeSingle) {
    // obtain attribute property (only for improve code legibility)
    const auto& attrvalue = GNEAttributeCarrier::getTagProperties(myAdditionalFrameParent->myItemSelector->getCurrentTypeTag()).getAttribute(AdditionalAttributeSingle);
    myVectorOfsingleAdditionalParameter.at(attrvalue.getPositionListed())->showAdditionalAttribute(AdditionalAttributeSingle, attrvalue.getDefaultValue());
}


void
GNEAdditionalFrame::AdditionalAttributes::showAdditionalAttributesModul() {
    recalc();
    show();
}


void
GNEAdditionalFrame::AdditionalAttributes::hideAdditionalAttributesModul() {
    hide();
}


void
GNEAdditionalFrame::AdditionalAttributes::getAttributesAndValues(std::map<SumoXMLAttr, std::string> &valuesMap) const {
    // get standard parameters
    for (int i = 0; i < (int)myVectorOfsingleAdditionalParameter.size(); i++) {
        if (myVectorOfsingleAdditionalParameter.at(i)->getAttr() != SUMO_ATTR_NOTHING) {
            valuesMap[myVectorOfsingleAdditionalParameter.at(i)->getAttr()] = myVectorOfsingleAdditionalParameter.at(i)->getValue();
        }
    }
}


void
GNEAdditionalFrame::AdditionalAttributes::showWarningMessage(std::string extra) const {
    std::string errorMessage;
    // iterate over standard parameters
    for (auto i : GNEAttributeCarrier::getTagProperties(myAdditionalFrameParent->myItemSelector->getCurrentTypeTag())) {
        if (errorMessage.empty()) {
            // Return string with the error if at least one of the parameter isn't valid
            std::string attributeValue = myVectorOfsingleAdditionalParameter.at(i.second.getPositionListed())->isAttributeValid();
            if (attributeValue.size() != 0) {
                errorMessage = attributeValue;
            }
        }
    }
    // show warning box if input parameters aren't invalid
    if (extra.size() == 0) {
        errorMessage = "Invalid input parameter of " + toString(myAdditionalFrameParent->myItemSelector->getCurrentTypeTag()) + ": " + errorMessage;
    } else {
        errorMessage = "Invalid input parameter of " + toString(myAdditionalFrameParent->myItemSelector->getCurrentTypeTag()) + ": " + extra;
    }

    // set message in status bar
    myAdditionalFrameParent->getViewNet()->setStatusBarText(errorMessage);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(errorMessage);
}


bool
GNEAdditionalFrame::AdditionalAttributes::areCurrentAdditionalAttributesValid() const {
    // iterate over standar parameters
    for (auto i : GNEAttributeCarrier::getTagProperties(myAdditionalFrameParent->myItemSelector->getCurrentTypeTag())) {
        // Return false if error message of attriuve isn't empty
        if (myVectorOfsingleAdditionalParameter.at(i.second.getPositionListed())->isAttributeValid().size() != 0) {
            return false;
        }
    }
    return true;
}


long
GNEAdditionalFrame::AdditionalAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog
    myAdditionalFrameParent->openHelpAttributesDialog(myAdditionalFrameParent->myItemSelector->getCurrentTypeTag());
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorLaneParents - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorLaneParents::SelectorLaneParents(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Lane Selector", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // create start and stop buttons
    myStopSelectingButton = new FXButton(this, "Stop selecting", nullptr, this, MID_GNE_ADDITIONALFRAME_STOPSELECTION, GUIDesignButton);
    myAbortSelectingButton = new FXButton(this, "Abort selecting", nullptr, this, MID_GNE_ADDITIONALFRAME_ABORTSELECTION, GUIDesignButton);
    // disable stop and abort functions as init
    myStopSelectingButton->disable();
    myAbortSelectingButton->disable();
    // define colors
    myCandidateLaneColor = RGBColor(0, 64, 0, 255);
    mySelectedLaneColor = RGBColor::GREEN;
}


GNEAdditionalFrame::SelectorLaneParents::~SelectorLaneParents() {}


void 
GNEAdditionalFrame::SelectorLaneParents::showSelectorLaneParentsModul() {
    // abort current selection before show
    abortConsecutiveLaneSelector();
    // show FXGroupBox
    FXGroupBox::show();
}


void 
GNEAdditionalFrame::SelectorLaneParents::hideSelectorLaneParentsModul() {
    // abort current selection before hide
    abortConsecutiveLaneSelector();
    // hide FXGroupBox
    FXGroupBox::hide();
}


void 
GNEAdditionalFrame::SelectorLaneParents::startConsecutiveLaneSelector(GNELane *lane, const Position &clickedPosition) {
    // Only start selection if SelectorLaneParents modul is shown
    if (shown()) {
        // change buttons
        myStopSelectingButton->enable();
        myAbortSelectingButton->enable();
        // add lane
        addSelectedLane(lane, clickedPosition);
    }
}


bool 
GNEAdditionalFrame::SelectorLaneParents::stopConsecutiveLaneSelector() {
    // obtain tagproperty (only for improve code legibility)
    const auto& tagValues = GNEAttributeCarrier::getTagProperties(myAdditionalFrameParent->myItemSelector->getCurrentTypeTag());
    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap;
    // abort if there isn't at least two lanes
    if (mySelectedLanes.size() < 2) {
        WRITE_WARNING(toString(myAdditionalFrameParent->myItemSelector->getCurrentTypeTag()) + " requieres at least two lanes.");
        // abort consecutive lane selector
        abortConsecutiveLaneSelector();
        return false;
    }
    // fill valuesOfElement with Additional attributes from Frame
    myAdditionalFrameParent->myAdditionalAttributes->getAttributesAndValues(valuesMap);
    // fill valuesOfElement with Netedit attributes from Frame
    myAdditionalFrameParent->myNeteditAttributes->getNeteditAttributesAndValues(valuesMap, nullptr);
    // Generate id of element
    valuesMap[SUMO_ATTR_ID] = myAdditionalFrameParent->generateID(nullptr);
    // obtain lane IDs
    std::vector<std::string> laneIDs;
    for (auto i : mySelectedLanes) {
        laneIDs.push_back(i.first->getID());
    }
    valuesMap[SUMO_ATTR_LANES] = joinToString(laneIDs, " ");
    // Obtain clicked position over first lane
    valuesMap[SUMO_ATTR_POSITION] = toString(mySelectedLanes.front().second);
    // Obtain clicked position over last lane
    valuesMap[SUMO_ATTR_ENDPOS] = toString(mySelectedLanes.back().second);
    // parse common attributes
    if(!myAdditionalFrameParent->buildAdditionalCommonAttributes(valuesMap, tagValues)) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalFrameParent->myAdditionalAttributes->areCurrentAdditionalAttributesValid() == false) {
        myAdditionalFrameParent->myAdditionalAttributes->showWarningMessage();
        return false;
    } else if (GNEAdditionalHandler::buildAdditional(myAdditionalFrameParent->myViewNet, true, myAdditionalFrameParent->myItemSelector->getCurrentTypeTag(), valuesMap)) {
        // abort consecutive lane selector
        abortConsecutiveLaneSelector();
        return true;
    } else {
        return false;
    }
}


void 
GNEAdditionalFrame::SelectorLaneParents::abortConsecutiveLaneSelector() {
     // reset color of all candidate lanes
    for (auto i : myCandidateLanes) {
        i->setSpecialColor(nullptr);
    }
    // clear candidate colors
    myCandidateLanes.clear();
    // reset color of all selected lanes
    for (auto i : mySelectedLanes) {
        i.first->setSpecialColor(nullptr);
    }
    // clear selected lanes
    mySelectedLanes.clear();
    // disable buttons
    myStopSelectingButton->disable();
    myAbortSelectingButton->disable();
    // update view (due colors)
    myAdditionalFrameParent->getViewNet()->update();
}


bool 
GNEAdditionalFrame::SelectorLaneParents::addSelectedLane(GNELane *lane, const Position &clickedPosition) {
    // first check that lane exist
    if(lane == nullptr) {
        return false;
    }
    // check that lane wasn't already selected
    for (auto i : mySelectedLanes) {
        if (i.first == lane) {
            WRITE_WARNING("Duplicated lanes aren't allowed");
            return false;
        }
    }
    // check that there is candidate lanes
    if(mySelectedLanes.size() > 0) {
        if (myCandidateLanes.empty()) {
            WRITE_WARNING("Only candidate lanes are allowed");
            return false;
        } else if((myCandidateLanes.size() > 0) && (std::find(myCandidateLanes.begin(), myCandidateLanes.end(), lane) == myCandidateLanes.end())) {
            WRITE_WARNING("Only consecutive lanes are allowed");
            return false;
        }
    }
    // select lane and save the clicked position
    mySelectedLanes.push_back(std::make_pair(lane, lane->getShape().nearest_offset_to_point2D(clickedPosition) / lane->getLengthGeometryFactor()));
    // change color of selected lane
    lane->setSpecialColor(&mySelectedLaneColor);
    // restore original color of candidates (except already selected)
    for (auto i : myCandidateLanes) {
        if(!isLaneSelected(i)) {
            i->setSpecialColor(nullptr);
        }
    }
    // clear candidate lanes
    myCandidateLanes.clear();
    // fill candidate lanes
    for (auto i : lane->getParentEdge().getGNEConnections()) {
        // check that possible candidate lane isn't already selected 
        if((lane == i->getLaneFrom()) && (!isLaneSelected(i->getLaneTo()))) {
            // set candidate lane
            i->getLaneTo()->setSpecialColor(&myCandidateLaneColor);
            myCandidateLanes.push_back(i->getLaneTo());
        }
    }
    // update view (due colors)
    myAdditionalFrameParent->getViewNet()->update();
    return true;
}


void 
GNEAdditionalFrame::SelectorLaneParents::removeLastSelectedLane() {
    if(mySelectedLanes.size() > 1) {
        mySelectedLanes.pop_back();
    } else {
        WRITE_WARNING("First lane cannot be removed");
    }
}


bool 
GNEAdditionalFrame::SelectorLaneParents::isSelectingLanes() const {
    return myStopSelectingButton->isEnabled();
}


bool 
GNEAdditionalFrame::SelectorLaneParents::isShown() const {
    return shown();
}


const RGBColor&
GNEAdditionalFrame::SelectorLaneParents::getSelectedLaneColor() const {
    return mySelectedLaneColor;
}


const std::vector<std::pair<GNELane*, double> >&
GNEAdditionalFrame::SelectorLaneParents::getSelectedLanes() const {
    return mySelectedLanes;
}


long
GNEAdditionalFrame::SelectorLaneParents::onCmdStopSelection(FXObject*, FXSelector, void*) {
    stopConsecutiveLaneSelector();
    return 0;
}


long 
GNEAdditionalFrame::SelectorLaneParents::onCmdAbortSelection(FXObject*, FXSelector, void*) {
    abortConsecutiveLaneSelector();
    return 0;
}


bool
GNEAdditionalFrame::SelectorLaneParents::isLaneSelected(GNELane *lane) const {
    for (auto i : mySelectedLanes) {
        if (i.first == lane) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorAdditionalParent - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorAdditionalParent::SelectorAdditionalParent(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Parent selector", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent),
    myAdditionalTypeParent(SUMO_TAG_NOTHING) {
    // Create label with the type of SelectorAdditionalParent
    myFirstAdditionalParentsLabel = new FXLabel(this, "No additional selected", nullptr, GUIDesignLabelLeftThick);
    // Create list
    myFirstAdditionalParentsList = new FXList(this, this, MID_GNE_SET_TYPE, GUIDesignListSingleElement, 0, 0, 0, 100);
    // Hide List
    hideSelectorAdditionalParentModul();
}


GNEAdditionalFrame::SelectorAdditionalParent::~SelectorAdditionalParent() {}


std::string
GNEAdditionalFrame::SelectorAdditionalParent::getIdSelected() const {
    for (int i = 0; i < myFirstAdditionalParentsList->getNumItems(); i++) {
        if (myFirstAdditionalParentsList->isItemSelected(i)) {
            return myFirstAdditionalParentsList->getItem(i)->getText().text();
        }
    }
    return "";
}


void
GNEAdditionalFrame::SelectorAdditionalParent::setIDSelected(const std::string& id) {
    // first unselect all
    for (int i = 0; i < myFirstAdditionalParentsList->getNumItems(); i++) {
        myFirstAdditionalParentsList->getItem(i)->setSelected(false);
    }
    // select element if correspond to given ID
    for (int i = 0; i < myFirstAdditionalParentsList->getNumItems(); i++) {
        if (myFirstAdditionalParentsList->getItem(i)->getText().text() == id) {
            myFirstAdditionalParentsList->getItem(i)->setSelected(true);
        }
    }
    // recalc myFirstAdditionalParentsList
    myFirstAdditionalParentsList->recalc();
}


bool
GNEAdditionalFrame::SelectorAdditionalParent::showSelectorAdditionalParentModul(SumoXMLTag additionalType) {
    // make sure that we're editing an additional tag
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TAGProperty::TAGPROPERTY_ADDITIONAL, false);
    for (auto i : listOfTags) {
        if (i == additionalType) {
            myAdditionalTypeParent = additionalType;
            myFirstAdditionalParentsLabel->setText(("Parent type: " + toString(additionalType)).c_str());
            refreshSelectorAdditionalParentModul();
            show();
            return true;
        }
    }
    return false;
}


void
GNEAdditionalFrame::SelectorAdditionalParent::hideSelectorAdditionalParentModul() {
    myAdditionalTypeParent = SUMO_TAG_NOTHING;
    hide();
}


void
GNEAdditionalFrame::SelectorAdditionalParent::refreshSelectorAdditionalParentModul() {
    myFirstAdditionalParentsList->clearItems();
    if (myAdditionalTypeParent != SUMO_TAG_NOTHING) {
        // fill list with IDs of additionals
        for (auto i : myAdditionalFrameParent->getViewNet()->getNet()->getAdditionalByType(myAdditionalTypeParent)) {
            myFirstAdditionalParentsList->appendItem(i.first.c_str());
        }
    }
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorEdgeChilds - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorEdgeChilds::SelectorEdgeChilds(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Edges", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // Create menuCheck for selected edges
    myUseSelectedEdgesCheckButton = new FXCheckButton(this, ("Use selected " + toString(SUMO_TAG_EDGE) + "s").c_str(), this, MID_GNE_ADDITIONALFRAME_USESELECTED, GUIDesignCheckButtonAttribute);

    // Create search box
    myEdgesSearch = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_SEARCH, GUIDesignTextField);

    // Create list
    myList = new FXList(this, this, MID_GNE_ADDITIONALFRAME_SELECT, GUIDesignList, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create button for clear selection
    myClearEdgesSelection = new FXButton(buttonsFrame, "Clear", nullptr, this, MID_GNE_ADDITIONALFRAME_CLEARSELECTION, GUIDesignButtonRectangular);

    // Create button for invert selection
    myInvertEdgesSelection = new FXButton(buttonsFrame, "Invert", nullptr, this, MID_GNE_ADDITIONALFRAME_INVERTSELECTION, GUIDesignButtonRectangular);

    // Hide List
    hideSelectorEdgeChildsModul();
}


GNEAdditionalFrame::SelectorEdgeChilds::~SelectorEdgeChilds() {}


std::string
GNEAdditionalFrame::SelectorEdgeChilds::getEdgeIdsSelected() const {
    std::vector<std::string> vectorOfIds;
    if (myUseSelectedEdgesCheckButton->getCheck()) {
        // get Selected edges
        std::vector<GNEEdge*> selectedEdges = myAdditionalFrameParent->getViewNet()->getNet()->retrieveEdges(true);
        // Iterate over selectedEdges and getId
        for (auto i : selectedEdges) {
            vectorOfIds.push_back(i->getID());
        }
    } else {
        // Obtain Id's of list
        for (int i = 0; i < myList->getNumItems(); i++) {
            if (myList->isItemSelected(i)) {
                vectorOfIds.push_back(myList->getItem(i)->getText().text());
            }
        }
    }
    return joinToString(vectorOfIds, " ");
}


void
GNEAdditionalFrame::SelectorEdgeChilds::showSelectorEdgeChildsModul(std::string search) {
    // clear list of egdge ids
    myList->clearItems();
    // get all edges of net
    /// @todo this function must be improved.
    std::vector<GNEEdge*> vectorOfEdges = myAdditionalFrameParent->getViewNet()->getNet()->retrieveEdges(false);
    // iterate over edges of net
    for (auto i : vectorOfEdges) {
        // If search criterium is correct, then append ittem
        if (i->getID().find(search) != std::string::npos) {
            myList->appendItem(i->getID().c_str());
        }
    }
    // By default, CheckBox for useSelectedEdges isn't checked
    myUseSelectedEdgesCheckButton->setCheck(false);
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    // Show dialog
    show();
}


void
GNEAdditionalFrame::SelectorEdgeChilds::hideSelectorEdgeChildsModul() {
    FXGroupBox::hide();
}


void
GNEAdditionalFrame::SelectorEdgeChilds::updateUseSelectedEdges() {
    // Enable or disable use selected edges
    if (myAdditionalFrameParent->getViewNet()->getNet()->retrieveEdges(true).size() > 0) {
        myUseSelectedEdgesCheckButton->enable();
    } else {
        myUseSelectedEdgesCheckButton->disable();
    }
}


long
GNEAdditionalFrame::SelectorEdgeChilds::onCmdUseSelectedEdges(FXObject*, FXSelector, void*) {
    if (myUseSelectedEdgesCheckButton->getCheck()) {
        myEdgesSearch->hide();
        myList->hide();
        myClearEdgesSelection->hide();
        myInvertEdgesSelection->hide();
    } else {
        myEdgesSearch->show();
        myList->show();
        myClearEdgesSelection->show();
        myInvertEdgesSelection->show();
    }
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    return 1;
}


long
GNEAdditionalFrame::SelectorEdgeChilds::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorEdgeChilds that contains the searched string
    showSelectorEdgeChildsModul(myEdgesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::SelectorEdgeChilds::onCmdSelectEdge(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::SelectorEdgeChilds::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::SelectorEdgeChilds::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        } else {
            myList->selectItem(i);
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorLaneChilds - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorLaneChilds::SelectorLaneChilds(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Lanes", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // Create CheckBox for selected lanes
    myUseSelectedLanesCheckButton = new FXCheckButton(this, ("Use selected " + toString(SUMO_TAG_LANE) + "s").c_str(), this, MID_GNE_ADDITIONALFRAME_USESELECTED, GUIDesignCheckButtonAttribute);

    // Create search box
    myLanesSearch = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_SEARCH, GUIDesignTextField);

    // Create list
    myList = new FXList(this, this, MID_GNE_ADDITIONALFRAME_SELECT, GUIDesignList, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create button for clear selection
    clearLanesSelection = new FXButton(buttonsFrame, "clear", nullptr, this, MID_GNE_ADDITIONALFRAME_CLEARSELECTION, GUIDesignButtonRectangular);

    // Create button for invert selection
    invertLanesSelection = new FXButton(buttonsFrame, "invert", nullptr, this, MID_GNE_ADDITIONALFRAME_INVERTSELECTION, GUIDesignButtonRectangular);

    // Hide List
    hideSelectorLaneChildsModul();
}


GNEAdditionalFrame::SelectorLaneChilds::~SelectorLaneChilds() {}


std::string
GNEAdditionalFrame::SelectorLaneChilds::getLaneIdsSelected() const {
    std::vector<std::string> vectorOfIds;
    if (myUseSelectedLanesCheckButton->getCheck()) {
        // get Selected lanes
        std::vector<GNELane*> selectedLanes = myAdditionalFrameParent->getViewNet()->getNet()->retrieveLanes(true);
        // Iterate over selectedLanes and getId
        for (auto i : selectedLanes) {
            vectorOfIds.push_back(i->getID());
        }
    } else {
        // Obtain Id's of list
        for (int i = 0; i < myList->getNumItems(); i++) {
            if (myList->isItemSelected(i)) {
                vectorOfIds.push_back(myList->getItem(i)->getText().text());
            }
        }
    }
    return joinToString(vectorOfIds, " ");
}


void
GNEAdditionalFrame::SelectorLaneChilds::showSelectorLaneChildsModul(std::string search) {
    myList->clearItems();
    std::vector<GNELane*> vectorOfLanes = myAdditionalFrameParent->getViewNet()->getNet()->retrieveLanes(false);
    for (auto i : vectorOfLanes) {
        if (i->getID().find(search) != std::string::npos) {
            myList->appendItem(i->getID().c_str());
        }
    }
    // By default, CheckBox for useSelectedLanes isn't checked
    myUseSelectedLanesCheckButton->setCheck(false);
    // Show list
    show();
}


void
GNEAdditionalFrame::SelectorLaneChilds::hideSelectorLaneChildsModul() {
    FXGroupBox::hide();
}


void
GNEAdditionalFrame::SelectorLaneChilds::updateUseSelectedLanes() {
    // Enable or disable use selected Lanes
    if (myAdditionalFrameParent->getViewNet()->getNet()->retrieveLanes(true).size() > 0) {
        myUseSelectedLanesCheckButton->enable();
    } else {
        myUseSelectedLanesCheckButton->disable();
    }
}


long
GNEAdditionalFrame::SelectorLaneChilds::onCmdUseSelectedLanes(FXObject*, FXSelector, void*) {
    if (myUseSelectedLanesCheckButton->getCheck()) {
        myLanesSearch->hide();
        myList->hide();
        clearLanesSelection->hide();
        invertLanesSelection->hide();
    } else {
        myLanesSearch->show();
        myList->show();
        clearLanesSelection->show();
        invertLanesSelection->show();
    }
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    return 1;
}


long
GNEAdditionalFrame::SelectorLaneChilds::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorLaneChilds that contains the searched string
    showSelectorLaneChildsModul(myLanesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::SelectorLaneChilds::onCmdSelectLane(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::SelectorLaneChilds::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::SelectorLaneChilds::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        } else {
            myList->selectItem(i);
        }
    }
    return 1;
}

// ===========================================================================
// method definitions
// ===========================================================================

GNEAdditionalFrame::GNEAdditionalFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Additionals") {

    // create item Selector modul for additionals
    myItemSelector = new ItemSelector(this, GNEAttributeCarrier::TAGProperty::TAGPROPERTY_ADDITIONAL);

    // Create additional parameters
    myAdditionalAttributes = new AdditionalAttributes(this);

    // Create Netedit parameter
    myNeteditAttributes = new NeteditAttributes(this);

    // Create consecutive Lane Selector
    mySelectorLaneParents = new SelectorLaneParents(this);

    // Create create list for additional Set
    mySelectorAdditionalParent = new SelectorAdditionalParent(this);

    /// Create list for SelectorEdgeChilds
    mySelectorEdgeChilds = new SelectorEdgeChilds(this);

    /// Create list for SelectorLaneChilds
    mySelectorLaneChilds = new SelectorLaneChilds(this);

    // set BusStop as default additional
    myItemSelector->setCurrentTypeTag(SUMO_TAG_BUS_STOP);
}


GNEAdditionalFrame::~GNEAdditionalFrame() {}


bool
GNEAdditionalFrame::addAdditional(const GNEViewNet::ObjectsUnderCursor &objectsUnderCursor) {
    // first check that current selected additional is valid
    if (myItemSelector->getCurrentTypeTag() == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected additional isn't valid.");
        return false;
    }
    
    // obtain tagproperty (only for improve code legibility)
    const auto& tagValues = GNEAttributeCarrier::getTagProperties(myItemSelector->getCurrentTypeTag());

    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap;

    // fill valuesOfElement with attributes from Frame
    myAdditionalAttributes->getAttributesAndValues(valuesMap);

    // fill netedit attributes
    if(!myNeteditAttributes->getNeteditAttributesAndValues(valuesMap, objectsUnderCursor.lane)) {
        return false;
    }

    // If element owns an additional parent, get id of parent from AdditionalParentSelector
    if (tagValues.hasParent() && !buildAdditionalWithParent(valuesMap, objectsUnderCursor.additional, tagValues)) {
        return false;
    }
    // If consecutive Lane Selector is enabled, it means that either we're selecting lanes or we're finished or we'rent started
    if(tagValues.canBePlacedOverEdge()) {
        return buildAdditionalOverEdge(valuesMap, objectsUnderCursor.lane, tagValues);
    } else if(tagValues.canBePlacedOverLane()) {
        return buildAdditionalOverLane(valuesMap, objectsUnderCursor.lane, tagValues);
    } else if(tagValues.canBePlacedOverLanes()) {
        return buildAdditionalOverLanes(valuesMap, objectsUnderCursor.lane, tagValues);
    } else {
        return buildAdditionalOverView(valuesMap, tagValues);
    }
}

void
GNEAdditionalFrame::removeAdditional(GNEAdditional* additional) {
    myViewNet->getUndoList()->p_begin("delete " + toString(additional->getTag()));
    // first remove all additional childs of this additional calling this function recursively
    while (additional->getAdditionalChilds().size() > 0) {
        removeAdditional(additional->getAdditionalChilds().front());
    }
    // remove additional
    myViewNet->getUndoList()->add(new GNEChange_Additional(additional, false), true);
    myViewNet->getUndoList()->p_end();
}


void
GNEAdditionalFrame::showSelectorLaneChildsModul() {
    // Show frame
    GNEFrame::show();
    // Update UseSelectedLane CheckBox
    mySelectorEdgeChilds->updateUseSelectedEdges();
    // Update UseSelectedLane CheckBox
    mySelectorLaneChilds->updateUseSelectedLanes();
}


GNEAdditionalFrame::SelectorLaneParents* 
GNEAdditionalFrame::getConsecutiveLaneSelector() const {
    return mySelectorLaneParents;
}


void 
GNEAdditionalFrame::enableModuls(const GNEAttributeCarrier::TagValues &tagProperties) {
     // show NeteeditAttributes
    myNeteditAttributes->showNeteditAttributesModul(tagProperties);
    // Clear internal attributes
    myAdditionalAttributes->clearAttributes();
    // iterate over attributes of myCurrentAdditionalType
    for (auto i : tagProperties) {
        // only show attributes that aren't uniques
        if (!i.second.isUnique()) {
            myAdditionalAttributes->addAttribute(i.first);
        }
    }
    // show additional attribute modul
    myAdditionalAttributes->showAdditionalAttributesModul();
    // Show myAdditionalFrameParent if we're adding a additional with parent
    if (tagProperties.hasParent()) {
        mySelectorAdditionalParent->showSelectorAdditionalParentModul(tagProperties.getParentTag());
    } else {
        mySelectorAdditionalParent->hideSelectorAdditionalParentModul();
    }
    // Show SelectorEdgeChilds if we're adding an additional that own the attribute SUMO_ATTR_EDGES
    if (tagProperties.hasAttribute(SUMO_ATTR_EDGES)) {
        mySelectorEdgeChilds->showSelectorEdgeChildsModul();
    } else {
        mySelectorEdgeChilds->hideSelectorEdgeChildsModul();
    }
    // Show SelectorLaneChilds or consecutive lane selector if we're adding an additional that own the attribute SUMO_ATTR_LANES
    if (tagProperties.hasAttribute(SUMO_ATTR_LANES)) {
        if(tagProperties.hasParent() && tagProperties.getParentTag() == SUMO_TAG_LANE) {
            // show selector lane parent and hide selector lane child
            mySelectorLaneParents->showSelectorLaneParentsModul();
            mySelectorLaneChilds->hideSelectorLaneChildsModul();
        } else {
            // show selector lane child and hide selector lane parent
            mySelectorLaneChilds->showSelectorLaneChildsModul();
            mySelectorLaneParents->hideSelectorLaneParentsModul();
        }
    } else {
        mySelectorLaneChilds->hideSelectorLaneChildsModul();
        mySelectorLaneParents->hideSelectorLaneParentsModul();
    }
}


void 
GNEAdditionalFrame::disableModuls() {
    // hide all moduls if additional isn't valid
    myAdditionalAttributes->hideAdditionalAttributesModul();
    myNeteditAttributes->hideNeteditAttributesModul();
    mySelectorAdditionalParent->hideSelectorAdditionalParentModul();
    mySelectorEdgeChilds->hideSelectorEdgeChildsModul();
    mySelectorLaneChilds->hideSelectorLaneChildsModul();
    mySelectorLaneParents->hideSelectorLaneParentsModul();
}


std::string
GNEAdditionalFrame::generateID(GNENetElement* netElement) const {
    // obtain current number of additionals to generate a new index faster
    int additionalIndex = myViewNet->getNet()->getNumberOfAdditionals(myItemSelector->getCurrentTypeTag());
    std::string currentAdditionalTypeStr = toString(myItemSelector->getCurrentTypeTag());
    if (netElement) {
        // generate ID using netElement
        while (myViewNet->getNet()->retrieveAdditional(myItemSelector->getCurrentTypeTag(), currentAdditionalTypeStr + "_" + netElement->getID() + "_" + toString(additionalIndex), false) != nullptr) {
            additionalIndex++;
        }
        return currentAdditionalTypeStr + "_" + netElement->getID() + "_" + toString(additionalIndex);
    } else {
        // generate ID without netElement
        while (myViewNet->getNet()->retrieveAdditional(myItemSelector->getCurrentTypeTag(), currentAdditionalTypeStr + "_" + toString(additionalIndex), false) != nullptr) {
            additionalIndex++;
        }
        return currentAdditionalTypeStr + "_" + toString(additionalIndex);
    }
}


bool 
GNEAdditionalFrame::buildAdditionalWithParent(std::map<SumoXMLAttr, std::string> &valuesMap, GNEAdditional* additionalParent, const GNEAttributeCarrier::TagValues &tagValues) {
    // if user click over an additional element parent, mark int in AdditionalParentSelector
    if (additionalParent && (additionalParent->getTag() == tagValues.getParentTag())) {
        valuesMap[GNE_ATTR_PARENT] = additionalParent->getID();
        mySelectorAdditionalParent->setIDSelected(additionalParent->getID());
    }
    // stop if currently there isn't a valid selected parent
    if (mySelectorAdditionalParent->getIdSelected() != "") {
        valuesMap[GNE_ATTR_PARENT] = mySelectorAdditionalParent->getIdSelected();
    } else {
        myAdditionalAttributes->showWarningMessage("A " + toString(tagValues.getParentTag()) + " must be selected before insertion of " + toString(myItemSelector->getCurrentTypeTag()) + ".");
        return false;
    }
    return true;
}


bool 
GNEAdditionalFrame::buildAdditionalCommonAttributes(std::map<SumoXMLAttr, std::string> &valuesMap, const GNEAttributeCarrier::TagValues &tagValues) {
    // If additional has a interval defined by a begin or end, check that is valid
    if (tagValues.hasAttribute(SUMO_ATTR_STARTTIME) && tagValues.hasAttribute(SUMO_ATTR_END)) {
        double begin = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_STARTTIME]);
        double end = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_END]);
        if (begin > end) {
            myAdditionalAttributes->showWarningMessage("Attribute '" + toString(SUMO_ATTR_STARTTIME) + "' cannot be greater than attribute '" + toString(SUMO_ATTR_END) + "'.");
            return false;
        }
    }
    // If additional own the attribute SUMO_ATTR_FILE but was't defined, will defined as <ID>.xml
    if (tagValues.hasAttribute(SUMO_ATTR_FILE) && valuesMap[SUMO_ATTR_FILE] == "") {
        if ((myItemSelector->getCurrentTypeTag() != SUMO_TAG_CALIBRATOR) && (myItemSelector->getCurrentTypeTag() != SUMO_TAG_REROUTER)) {
            // SUMO_ATTR_FILE is optional for calibrators and rerouters (fails to load in sumo when given and the file does not exist)
            valuesMap[SUMO_ATTR_FILE] = (valuesMap[SUMO_ATTR_ID] + ".xml");
        }
    }
    // If element own a list of SelectorEdgeChilds as attribute
    if (tagValues.hasAttribute(SUMO_ATTR_EDGES) && !tagValues.canBePlacedOverEdges()) {
        // obtain edge IDs
        valuesMap[SUMO_ATTR_EDGES] = mySelectorEdgeChilds->getEdgeIdsSelected();
        // check if attribute has at least one edge
        if (valuesMap[SUMO_ATTR_EDGES] == "") {
            myAdditionalAttributes->showWarningMessage("List of " + toString(SUMO_TAG_EDGE) + "s cannot be empty");
            return false;
        }
    }
    // get values of mySelectorLaneChilds, if tag correspond to an element that has lanes as childs
    if (tagValues.hasAttribute(SUMO_ATTR_LANES) && !tagValues.canBePlacedOverLanes()) {
        // obtain lane IDs
        valuesMap[SUMO_ATTR_LANES] = mySelectorLaneChilds->getLaneIdsSelected();
        // check if attribute has at least a lane
        if (valuesMap[SUMO_ATTR_LANES] == "") {
            myAdditionalAttributes->showWarningMessage("List of " + toString(SUMO_TAG_LANE) + "s cannot be empty");
            return false;
        }
    }
    // all ok, continue building additionals
    return true;
}


bool 
GNEAdditionalFrame::buildAdditionalOverEdge(std::map<SumoXMLAttr, std::string> &valuesMap, GNELane* lane, const GNEAttributeCarrier::TagValues &tagValues) {
    // check that edge exist
    if (lane) {
        // Get attribute lane's edge
        valuesMap[SUMO_ATTR_EDGE] = lane->getParentEdge().getID();
        // Generate id of element based on the lane's edge
        valuesMap[SUMO_ATTR_ID] = generateID(&lane->getParentEdge());
    } else {
        return false;
    }
    // parse common attributes
    if(!buildAdditionalCommonAttributes(valuesMap, tagValues)) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalAttributes->areCurrentAdditionalAttributesValid() == false) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    } else if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myItemSelector->getCurrentTypeTag(), valuesMap)) {
        // Refresh additional Parent Selector (For additionals that have a limited number of childs)
        mySelectorAdditionalParent->refreshSelectorAdditionalParentModul();
        // clear selected eddges and lanes
        mySelectorEdgeChilds->onCmdClearSelection(nullptr, 0, nullptr);
        mySelectorLaneChilds->onCmdClearSelection(nullptr, 0, nullptr);
        return true;
    } else {
        return false;
    }
}


bool 
GNEAdditionalFrame::buildAdditionalOverLane(std::map<SumoXMLAttr, std::string> &valuesMap, GNELane* lane, const GNEAttributeCarrier::TagValues &tagValues) {
    // check that lane exist
    if (lane) {
        // Get attribute lane
        valuesMap[SUMO_ATTR_LANE] = lane->getID();
        // Generate id of element based on the lane
        valuesMap[SUMO_ATTR_ID] = generateID(lane);
    } else {
        return false;
    }
    // Obtain position of the mouse over lane (limited over grid)
    double mousePositionOverLane = lane->getShape().nearest_offset_to_point2D(myViewNet->snapToActiveGrid(myViewNet->getPositionInformation())) / lane->getLengthGeometryFactor();
    // Obtain position attribute if wasn't previously set in Frame
    if (tagValues.hasAttribute(SUMO_ATTR_POSITION) && (valuesMap.find(SUMO_ATTR_POSITION) == valuesMap.end())) {
        valuesMap[SUMO_ATTR_POSITION] = toString(mousePositionOverLane);
    }
    // parse common attributes
    if(!buildAdditionalCommonAttributes(valuesMap, tagValues)) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalAttributes->areCurrentAdditionalAttributesValid() == false) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    } else if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myItemSelector->getCurrentTypeTag(), valuesMap)) {
        // Refresh additional Parent Selector (For additionals that have a limited number of childs)
        mySelectorAdditionalParent->refreshSelectorAdditionalParentModul();
        // clear selected eddges and lanes
        mySelectorEdgeChilds->onCmdClearSelection(nullptr, 0, nullptr);
        mySelectorLaneChilds->onCmdClearSelection(nullptr, 0, nullptr);
        return true;
    } else {
        return false;
    }
}


 bool 
GNEAdditionalFrame::buildAdditionalOverLanes(std::map<SumoXMLAttr, std::string> &valuesMap, GNELane* lane, const GNEAttributeCarrier::TagValues &tagValues) {
     // stop if lane isn't valid
    if(lane == nullptr) {
        return false;
    }
    if(mySelectorLaneParents->isSelectingLanes()) {
        // select clicked lane, but don't build additional
        mySelectorLaneParents->addSelectedLane(lane, myViewNet->getPositionInformation());
        return false;
    } else if(mySelectorLaneParents->getSelectedLanes().empty()) {
        // if there isn't selected lanes, that means that we will be start selecting lanes
        mySelectorLaneParents->startConsecutiveLaneSelector(lane, myViewNet->getPositionInformation());
        return false;
    } else {
        // Generate id of element based on the first lane
        valuesMap[SUMO_ATTR_ID] = generateID(mySelectorLaneParents->getSelectedLanes().front().first);
        // obtain lane IDs
        std::vector<std::string> laneIDs;
        for (auto i : mySelectorLaneParents->getSelectedLanes()) {
            laneIDs.push_back(i.first->getID());
        }
        valuesMap[SUMO_ATTR_LANES] = joinToString(laneIDs, " ");
        // Check if clicked position over first lane has to be obtained
        if(tagValues.hasAttribute(SUMO_ATTR_POSITION)) {
            valuesMap[SUMO_ATTR_POSITION] = toString(mySelectorLaneParents->getSelectedLanes().front().second);
        }
        // Check if clicked position over last lane has to be obtained
        if(tagValues.hasAttribute(SUMO_ATTR_ENDPOS)) {
            valuesMap[SUMO_ATTR_ENDPOS] = toString(mySelectorLaneParents->getSelectedLanes().back().second);
        }
        // parse common attributes
        if(!buildAdditionalCommonAttributes(valuesMap, tagValues)) {
            return false;
        }
        // show warning dialogbox and stop check if input parameters are valid
        if (myAdditionalAttributes->areCurrentAdditionalAttributesValid() == false) {
            myAdditionalAttributes->showWarningMessage();
            return false;
        } else if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myItemSelector->getCurrentTypeTag(), valuesMap)) {
            // Refresh additional Parent Selector (For additionals that have a limited number of childs)
            mySelectorAdditionalParent->refreshSelectorAdditionalParentModul();
            // abort lane selector
            mySelectorLaneParents->abortConsecutiveLaneSelector();
            return true;
        } else {
            // additional cannot be build
            return false;
        }
    }
 }


bool
GNEAdditionalFrame::buildAdditionalOverView(std::map<SumoXMLAttr, std::string> &valuesMap, const GNEAttributeCarrier::TagValues &tagValues) {
    // Generate id of element
    valuesMap[SUMO_ATTR_ID] = generateID(nullptr);
    // Obtain position attribute if wasn't previously set in Frame
    if (tagValues.hasAttribute(SUMO_ATTR_POSITION) && (valuesMap.find(SUMO_ATTR_POSITION) == valuesMap.end())) {
        // An attribute "position" can be either a float or a Position. If isn't float, we get the position over map
        valuesMap[SUMO_ATTR_POSITION] = toString(myViewNet->snapToActiveGrid(myViewNet->getPositionInformation()));
    }
    // parse common attributes
    if(!buildAdditionalCommonAttributes(valuesMap, tagValues)) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalAttributes->areCurrentAdditionalAttributesValid() == false) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    } else if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myItemSelector->getCurrentTypeTag(), valuesMap)) {
        // Refresh additional Parent Selector (For additionals that have a limited number of childs)
        mySelectorAdditionalParent->refreshSelectorAdditionalParentModul();
        // clear selected eddges and lanes
        mySelectorEdgeChilds->onCmdClearSelection(nullptr, 0, nullptr);
        mySelectorLaneChilds->onCmdClearSelection(nullptr, 0, nullptr);
        return true;
    } else {
        return false;
    }
}

/****************************************************************************/

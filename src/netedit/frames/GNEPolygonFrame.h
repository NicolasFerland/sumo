/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPolygonFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2017
/// @version $Id$
///
// The Widget for add polygons
/****************************************************************************/
#ifndef GNEPolygonFrame_h
#define GNEPolygonFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include "GNEFrame.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNEPolygonFrame
* The Widget for setting internal attributes of shape elements
*/
class GNEPolygonFrame : public GNEFrame {

public:

    /// @brief enum with all possible values after try to create an shape using frame
    enum AddShapeResult {
        ADDSHAPE_SUCCESS,               // Shape was successfully created
        ADDSHAPE_UPDATEDTEMPORALSHAPE,  // Added or removed a new point to temporal shape
        ADDSHAPE_INVALID,               // Shape wasn't created
        ADDSHAPE_NOTHING                // Nothing to do
    };

    /**@brief Constructor
    * @brief parent FXHorizontalFrame in which this GNEFrame is placed
    * @brief viewNet viewNet that uses this GNEFrame
    */
    GNEPolygonFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEPolygonFrame();

    /**@brief process click over Viewnet
    * @param[in] clickedPosition clicked position over ViewNet
    * @param[in] lane clicked lane
    * @return AddShapeStatus with the result of operation
    */
    AddShapeResult processClick(const Position& clickedPosition, GNELane* lane);

    /// @brief show shape frame and update use selected edges/lanes
    void show();

    /// @brief get list of selecte id's in string format
    static std::string getIdsSelected(const FXList* list);

    /// @brief get drawing mode editor
    DrawingShape* getDrawingShape() const;

protected:
    /**@brief build a shaped element using the drawed shape
     * return true if was sucesfully created
     * @note called when user stop drawing polygon
     */
    bool buildShape();

     /// @brief enable moduls depending of item selected in ItemSelector
    void enableModuls(GNEAttributeCarrier::TagValues *tagProperties);

    /// @brief disable moduls if element selected in itemSelector isn't valid
    void disableModuls();

    /// @brief add Polygon
    bool addPolygon(const std::map<SumoXMLAttr, std::string>& POIValues);

    /// @brief add POI
    bool addPOI(const std::map<SumoXMLAttr, std::string>& POIValues);

    /// @brief add POILane
    bool addPOILane(const std::map<SumoXMLAttr, std::string>& POIValues);

private:
    /// @brief item selector
    ItemSelector* myItemSelector;

    /// @brief shape internal attributes
    ACAttributes* myShapeAttributes;

    /// @brief Netedit parameter
    NeteditAttributes* myNeteditAttributes;

    /// @brief Drawing shape
    DrawingShape* myDrawingShape;
};


#endif

/****************************************************************************/

.pragma library

.import QtQuick 2.15 as QtQuick

function traverse(list_view)
{
	var retval = new Map([[QtQuick.ListView.Beginning, 0], [QtQuick.ListView.End, 0]]);

	if (!(list_view instanceof QtQuick.ListView) || list_view.count <= 0) {
		return retval;
	}

	var axis = 0;
	var coordinate = 0;
	if (list_view.orientation === QtQuick.ListView.Horizontal) {
		axis = list_view.width;
		coordinate = list_view.contentX;
	} else if (list_view.orientation === QtQuick.ListView.Vertical) {
		axis = list_view.height;
		coordinate = list_view.contentY;
	} else {
		return retval;
	}

	var backward = false;
	var backward_offset = coordinate + axis;
	var backward_limit = coordinate;

	var forward = false;
	var forward_offset = coordinate;
	var forward_limit = coordinate + axis;

	do {
		if (!backward) {
			retval[QtQuick.ListView.End] = list_view.indexAt(1, backward_offset);
			if (retval[QtQuick.ListView.End] >= 0) {
				backward = true;
			} else if (retval[QtQuick.ListView.End] < 0 && axis <= 0) {
				backward = true;
				retval[QtQuick.ListView.End] = 0;
			} else {
				backward = (backward_offset -= list_view.spacing) < backward_limit;
				retval[QtQuick.ListView.End] = 0;
			}
		}
/*
		console.debug
		(
			"FeedView::traverse >",
			"backward_offset:", backward_offset,
			"backward_limit:", backward_limit,
			"coordinate:", coordinate,
			"axis:", axis
		);
*/
		if (!forward) {
			retval[QtQuick.ListView.Beginning] = list_view.indexAt(1, forward_offset);
			if (retval[QtQuick.ListView.Beginning] >= 0) {
				forward = true;
			} else if (retval[QtQuick.ListView.Beginning] < 0 && axis <= 0) {
				forward = true;
				retval[QtQuick.ListView.Beginning] = 0;
			} else {
				forward = (forward_offset += list_view.spacing) > forward_limit;
				retval[QtQuick.ListView.Beginning] = 0;
			}
		}
/*
		console.debug
		(
			"FeedView::traverse >",
			"forward_offset:", forward_offset,
			"forward_limit:", forward_limit,
			"coordinate:", coordinate,
			"axis:", axis
		);
*/
	} while (!forward || !backward);

	return retval;
}

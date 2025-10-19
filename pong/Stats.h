
bool click(float x1, float x2, float y1, float y2) {
	if ((Camera::state.mousendcX >= x1 && Camera::state.mousendcX <= x2) &&
		(Camera::state.mousendcY >= y1 && Camera::state.mousendcY <= y2))
		return true;
}


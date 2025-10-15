bool SSTAT = false;
bool click(float x1, float x2, float y1, float y2) {
	if ((Camera::state.mousendcX >= x1 && Camera::state.mousendcX <= x2) &&
		(Camera::state.mousendcY >= y1 && Camera::state.mousendcY <= y2))
		return true;
}

void showStat() {

	if (click(0.825, 0.975, 0.825, 0.975) && SSTAT == false)
		if (!SSTAT)
			SSTAT = true;
		else
			SSTAT = false;
	if (SSTAT) {
		Draw::DrawUIimage(5, 0.8, 1, 0.8, 1);
	}
}
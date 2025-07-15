#include <leveling.h>
#include <axis.h>

bool OnePosLeveler::step() {
    switch (state) {
        case 0: //give space, prep axis
            axis->suspend(); //suspend axis to stop all movement
            axis->generalMove({'r', 30, 0.25});
            break;
        case 1: //prep sensor, move down
            axis->levelSensor->prep();
            axis->generalMove({'r', -1000, 25});
            break;
        case 2: //zero, give space for round 2
            axis->stop();
            axis->levelSensor->stow();
            axis->zero();
            axis->generalMove({'r', 20, 0.8});
            break;
        case 3: //move down again
			axis->levelSensor->prep();
			axis->generalMove({'r', -1000, 300});
            break;
		case 4: //zero again, finish, return
            axis->stop();
            axis->levelSensor->stow();
            axis->zero();
			axis->resume(true); //resume axis, trash move queue
			state = 0; //reset state
            return true; //finished leveling
        default:
            break;
    }

    state++;
    return false;
}
bool OnePosLeveler::tick() {
    if (axis->levelSensor->detect() && state == 0) {
        axis->zero();
        return true;
    }

    switch (state) {
    case 0:
        break;
    case 1:
        if (!axis->moving) {
            break;
        }
        return false;
    case 2:
        if (axis->levelSensor->detect()) {
            break;
        }
        return false;
    case 3:
        if (!axis->moving) {
            break;
        }
        return false;
	case 4:
		if (axis->levelSensor->detect()) {
			break;
		}
		return false;
    default:
        return false;
    }

    return step();
}

Leveler* getLeveler(String type, Axis* axis) {
	Leveler* leveler = nullptr; //pointer to the leveler object
	if (type == "2Local") {

	} else if (type == "2Serial") {

	} else {
		leveler = new OnePosLeveler(axis);
	}
	return leveler;
}
<@setoutput path="${amName}.h" />
${bannerComment}

#ifndef ${(amName?upper_case)}_H_
#define ${(amName?upper_case)}_H_

#include <AnalogueModel.h>

class ${amName} : public AnalogueModel {
protected:
	<#if par1Name!="">${par1CType} ${par1Name};</#if>
	<#if par2Name!="">${par2CType} ${par2Name};</#if>

public:
	${amName}(<#if par1Name!="">${par1CType} ${par1Name}</#if><#if par2Name!="">,${par2CType} ${par2Name}</#if>);
	virtual ~${amName}();

	/**
	 * @brief Overriden by this implementation.
	 *
	 * Filters a specified AirFrame's Signal by adding an attenuation
	 * over time to the Signal.
	 *
	 * @param frame			The incomming frame.
	 * @param sendersPos	The position of the frame sender.
	 * @param receiverPos	The position of frame receiver.
	 */
	virtual void filterSignal(AirFrame *frame, const Coord& sendersPos, const Coord& receiverPos);
};

#endif /* ${(amName?upper_case)}_H_ */

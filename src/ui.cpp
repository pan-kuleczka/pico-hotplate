#include "ui.h"

ThermometerUI::ThermometerUI(Adafruit_GFX *display, float minTemp, float maxTemp)
{
    this->display = display;
    this->minTemp = minTemp;
    this->maxTemp = maxTemp;
}

void ThermometerUI::draw(int x, int y, float temperature)
{
    // Draw thermometer image
    display->drawBitmap(x, y, THERMOMETER_IMG, THERMOMETER_IMG_WIDTH, THERMOMETER_IMG_HEIGHT, 1, 0);

       
    // Draw temperature bar
    int maxHeight = 36;
    int beginX = x + 8;
    int width = 5;
    int height = (temperature - minTemp) / (maxTemp - minTemp) * maxHeight;
    if(height < 0) height = 0;
    if(height > maxHeight) height = maxHeight;
    int beginY = y + 45 - height;

    display->fillRect(beginX, beginY, width, height, 1);

    // Draw arrow pointing at the bar end
    display->fillTriangle(
        beginX - 2, beginY,
        beginX - 4, beginY - 2,
        beginX - 4, beginY + 2,
        1
    );

    // Draw temperature value
    display->setTextSize(1);
    display->setTextColor(1);
    std::string temperatureText = std::to_string((int)temperature);
    uint16_t textWidth, textHeight;
    int16_t textX, textY;
    display->getTextBounds(temperatureText.c_str(), 0, 0, &textX, &textY, &textWidth, &textHeight);
    display->setCursor(beginX - textWidth - 5, beginY - textHeight / 2);
    display->print(temperatureText.c_str());
}

void sleep(){

    if(count <= TIMEOUT) return;
    count = 0;
    pixels.clear();
    pixels.show();
    display.clearDisplay();
    display.display();
}

void timerint(){

    count ++;

}
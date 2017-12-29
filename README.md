# Scenario

### In DEU Cafeteria there is one cook during lunch times that put the food in the trays and puts the trays on the conveyor (a moving band that carries the filled trays out of the kitchen so students can fetch them and eat). Conveyor can take at most 8 trays. If the conveyor is full cook sleeps until a tray is fetched. Students come in random times and fetch their trays. If the there is no tray available they wait until cook fills a tray and puts it on the conveyor. For the sake of simplicity you don’t have to worry about the travel time of the tray on the conveyor.

 
Requirements <br />
* Simulation has to start with a full conveyor. (With 8 trays ready to be fetched) <br />
• There is only ONE (1) cook. (This means you will need a thread for the cook)
• If the conveyor is full cook sleeps, if not fills trays.
• Filling a tray takes random amount of time between 2-5 seconds.
• Each student has to be a different thread and has to be created at random times. (Creating constant amount of student threads prior to simulation is not ACCEPTED!!!)
• A student comes to cafeteria in random intervals that changes between 1-4 seconds.
• A student to fetch a ready tray takes exactly 1 second.
• One student can fetch a tray at a time (as it is in the real cafeteria).
• Order of the students while fetching a tray is NOT important!!!
• Your simulation is to be continuous (should not end unless it is stopped or killed)
• Your simulation needs to keep track of the
o The time cook started to sleep
o The time cook awake
o The time he started and finished to fill the nth tray o The time mth student arrived
o The time mth student fetched his tray
• Your simulation must keep the record of n (total number of trays filled), m (total students that came to cafeteria) that are given above and total number of students ate already (fetched tray).
• Each tread itself must increment or decrement those global variables. ( Not the thread creating code)
• Your simulation must give correct and detailed output of the times listed above, m and n at each state change as well as an illustration like the one given below. (30 points)
• Your simulation must have another “monitor” thread for printing out the illustration below at each state change. For printing the times, each thread (student or cook) must print out their own times.

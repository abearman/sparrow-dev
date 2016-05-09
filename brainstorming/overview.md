## Brainstorming: Warehouse Inventory Management

Sparrow's main product is **autonomous drone navigation in GPS-denied environments**. At our core, we have always been excited by the idea of creating an **environment-agnostic enabling technology**, with applications including warehouse inventory management, search and rescue operations, military reconnaissance, and more. During the first three months of our project, we considered warehouse inventory management as our first use case and had several brainstorming sessions on the idea of applying our product to the warehouse scenario.

**Why do we think this idea is exciting?**

We think that using drones in a warehouse setting provides multiple benefits, including the following:

- Integrate communication with vendors
- Can operate daily (vs. annually or 2x annually) at negligible additional cost
- Identify and fix physical and system inventory errors before they have business impact
- Generate real-time maps of your warehouse
- Report missing or stolen stock immediately
- Physical safety for workers (no people on forklifts)
- Reduce human error

**What were particularly exciting insights during our brainstorming and benchmarking?**

We discovered certain pain points.

- **Problem**: Customers misplace items. **Impact**: Lost revenue; this takes place daily at all hours of the day.
- **Problem**: Full inventory sweep is time-consuming. **Impact**: Safety, overtime pay, biannual or annual (legally mandated).
- **Problem**: Can’t do inventory very often. **Impact**: More up to date counts, can order low-stock items earlier.
- **Problem**: Item retrieval is slow. **Impact**: Wasted time to figure out where items went. **Solution**: Warehouse mapping

**What value are we adding?**

We considered Costco as a case study. The average Costco warehouse is 143,800 square feet... this is a huge space to walk over. Costco uses 40 groups of 2 people, working a week from 9 PM to 1 AM, twice a year, for its main full-warehouse inventory sweep. If you do the math, this comes out to 2240 man hours each time we take inventory, so 4480 man hours a year. With the average employee wage being $21/hour and $31.50 for overtime pay at 1.5x the normal rate, this comes out to $141,120 a year for a single warehouse to take inventory. There are 698 Costco warehouses, so this means $98,501,760 a year for Costco. If we compare this to the cost of 700 drones, where each one is $5000, this involves a one-time cost of $3,500,000. Thus, we have a **total added monetary value of $95,001,760** and **total added time of 3,127,040 hours**.

**What is our minimum viable product?**

Our minimum viable product would be a remote-controlled drone that takes photos as it goes along. We brainstormed a number of solutions involving the drone and involving computer vision.

Solutions involving the drone (ordered by best case to worst case):
- Fully automated navigation
- Path input
- Follow tape & input height
- Following a person
- Remote control

Solutions involving computer vision (ordered by best case to worst case):
- CV-based counting of items
- Scanning barcodes or RFIDs
- CV detection of item presence/absence
- Take photos for human inspection
 
**What products or solutions are doing something well that is related to our project?**

InventAIRy and DroneScan are the best we can find. These products assume it knows the height, and assumes uniform spacing. Currently these products do not involve autonomous drone navigation.

**Why is our team well positioned to do something great?**

Together, we have a variety of skills, including computer vision, AI, systems, and design. We think our diversity of skills makes us particularly well-suited to this project.

## Brainstorming: Search-and-Rescue Operations

However, at the beginning of spring quarter, as we began working on the actual technical challenges of building our product, we started to consider other markets where we thought our product could add more value. To that end, we decided to now shift our focus to another application: search and rescue (SAR), a very valuable military application. We realized that with the same technology we have been building, we would be able to provide much more value in this realm.

**What is the current SAR system & what is the cost?**

Last week, we attended the ARL + DIUX (Defense Innovation Unit Experimental) Conference at the Army Base on Moffett Field. As we know from our trip to the army base, one of the army's core values is to "never leave a fallen comrade." The current SAR process is to send multiple search and rescue teams out to look for a missing person, with each team starting from the last known point (LKP) of the missing person and scanning a certain radius away from this LKP. This is an immensely time-sensitive and personnel-intensive process, because the person could be in an unsafe environment or in a state of medical emergency.

Just last month, some of us were present during an SAR mission in San Francisco, responding to an overturned boat near Land's End. The process involved multiple Coast Guard boats, policemen, multiple firefighters including some standing on tall ladders trying to do surveillance, as well as a helicopter and crew -- easily 50 people all together. In addition, a large section of Highway 1 was blocked off. Watching the incident, it took over 20 minutes to just move the Coast Guard boats to the necessary area.

The US Coast Guard itself is the leader in SAR operations, coming to the assistance of an average of **114 people per day** at a total cost of **$680 million annually**, with **approximately 1000 lives lost annually**. A single Coast Guard patrol boat costs $1,147 per hour, and if the rescue requires the use of a C-30 turboprop plane, the bill increases at a rate of $7,600 per hour. (Although we have made the most research into the military application of SAR, other groups such as the National Park Service (NPS) is another agency heavily involved in SAR operations, logging almost 3,600 incidents of SAR a year, of which almost 150 result in fatalities, and causing almost $5 million in associated expenses.)

**Where do we come in and what is our value?**

There are several issues with the current system. SAR missions are both costly and dangerous. Environmental conditions can be unfriendly and make it difficult for the rescuers to traverse. It takes an inordinate amount of time to get in someone. Terrestrial robots and humans have difficulty in areas with unfriendly debris or terrain. Rural areas may have weak GPS signals, and using GPS means relying on external systems. It is difficult to operate in unmapped, unknown environments. It is also difficult to navigate through obstacles, such as trees or buildings.

Using drones in the SAR process would be invaluable, providing additional safety for SAR teams and dramatically decreasing the annual cost of SAR missions. Our integrated system allows the ability to navigate with or without GPS, and in so doing  decrease the cost needed to operate traditional fixed wing aircraft and helicopters and lowers the risk for humans. The reduction of human resources meas that SAR missions are no longer constrained by the size of the crew, and drones don't need rest (other than battery charging time, and in this case the drones can be switched out).

**What are some other tangible use cases?**

One recent example of these types of applications is the 2011 disaster at Japan's Fukushima Daiichi nuclear facility, where the Honeywell T-Hawk gathered up-close video and photos inside the plant in an effort to limit radiation releases. Having a drone would avoid exposing people to radiation. The drone's ability to get close access to damaged buildings surpasses the abilities of a helicopter or a human.

**With that said, who is our target customer?**

Our target customer is 47-year-old Stuart Young, Chief of the Asset Control and Behavior Branch of the Army Research Lab (ARL). He received his B.S. in Mechanical Engineering at the University of Washington, followed by his M.S. in Mechanical Engineering at the Naval Postgraduate School. He served 13 years in the Army Reserve as an Engineer Officer, was previously an engineer at NATO (North Atlantic Treaty Organization), and is now an engineer at the Army Research Laboratory, stationed in Maryland, where he has had over 24 years of experience managing and performing basic and applied research for autonomous military robotic systems. He is passionate about unmanned systems, UAVs, computer vision, and of course, military applications. Even though he is stationed in Maryland, he is often in California partnering with the DIUX, whose fundamental goal is to interface the Department of Defense (DoD) with the innovative startups, research labs, and companies in the Silicon Valley. At the DIUX + ARL conference at the army reserve in mid-April, Young expressed strong interest in our product and our team, speaking passionately about the huge potential for drone navigation in the spaces of search and rescue and reconnaissance. As an expert in military applications as well as UAVs, Young is the perfect target customer for us. We look forward to continuing to build our relationship with him and demo our product for him in the future.

##Needfinding

Some of the key insights we garnered from needfinding came from conversations and visits to various warehouses. We list some of the key highlights here.

**IKEA**

- When items come off of the truck, the count is given
- “Gatekeeper” position confirms the number of items on the truck and checks if things are damaged
- DOS system for recording stock, old black and white Microsoft system
- When item is sold, count decremented in system
- Live count kept on computer, but sometimes computer is down or doesn’t register when something is sold or damaged
- Sometimes the system doesn’t specify that the items have been put on the top shelves, so they can’t be found on the floor
- Department for handling logistics of inventory: visually inspect stock, sometimes with binoculars, to check if a spot is empty or not
- Inventory taken every week: one person counts empty spaces and compares with the record of spaces that are supposed to be empty, another person counts the full spaces
- If an item can’t be found because of a discrepancy in the system, it is flagged, and someone goes to look for it and correct the system (5 a.m. every day?)
- Barcodes scanned before putting items up on shelf (so barcodes facing out is not guaranteed)
- Can’t have forklifts running while store is open
- Things come on wooden pallets, and most are transferred to paper pallets
- Needs improvement: counting stock, faster fixes when customers flag an item as missing, numbers get messed up when store is open because customers move things around, physical stock check sometimes necessary

**Costco**

Problems we observed:
- Uneven heights of shelves
- Lack of barcodes on all projects
- Multiple barcodes
- Handwritten tags instead of a barcode
- Different items on a single pallet
- Some labeled “DNI” (Do Not Inventory)
- All their inventory is seasonal
- There is a distribution center in Hayward. Buyers come here to do floor walks, trying to determine what inventory Costco has and doesn’t have so they can compete by location
- Inventory management
- Major inventory is done twice a year
- Done during the hours of 9pm to 1am. People work all night shifts, and during open hours if they don’t finish
- 40 groups of 2 people. Teams of 2: one person counts, another person scans. This is basically double checking that the scan count is the same as the hand count.
- Scissor method, going all the way across the store (but no group gets ahead of the rest)
- First: inventory all boxes that are sealed. Second, do the boxes that are opened (i.e., because items went on overstock.com). Pull them down, unwrap them, count the items, and rewrap the box
- Liquor is hard: there’s a lot of it in refrigerated trucks outside, hard to inventory
- Day-to-day inventory:
- Basically this is restocking, happens at night with forklifts. 
- Perishables are inventoried every day (if they’re not good, they are individually scanned and disposed of). Costco makes its own compost out of its perishables

**Nestle**

- Switching to the new system might be labor intensive, but best way to do it in a short period would be to start tagging with the new system while the old system is still in place in the warehouse while manufacturers start adding them
- Optimization of warehouse layout 
- 3 categories of items
- High velocity (Labeled “A” or “1”) sells a lot
- Want them located close to distribution point to limit travel time, i.e. the loading dock. Where the order will be fulfilled.
- Market research to confirm items that are high velocity - interesting metric 
- Lower volume (“B”)
- Slow movers (“C”)
- Need to know the order of what’s going out, can see this by looking at the history of orders 
- Sales forecasting, market research to predict what items will be high velocity
- Categories determined by some thresholds of inventory turns (how fast things turn over)
- Method of evaluating warehouse performance
- Manpower per unit (how many hours it takes to load a truck, are fast-moving items near the truck/dock), this is a measure of warehouse efficiency and how long it takes to fulfill an order
- Cost per unit load (not paying overtime, no re-work)
- Types of sensors 
- Mostly barcodes or embedded RFIDs
- Don’t have RFIDs inside their packages at this point
- Nestle is a food company, so the individual packages aren’t very high value (not electronics or something). Would be a high cost to add a RFID transponder to every package. 
- RFID transponder on a pallet, tells the operator what class of item it is and where it should be stored, middleware between production/factory system and warehouse system
- Laser-guided automatic forktrucks
- Automated storage and retrieval systems, automated robotic warehouse system, high racking system to put pallets away and retrieve pallets for orders, no operators
- Drone-based solution for indoor navigation - info on environment of warehouse
- Depends on what types of items are being stored 
- Pharmaceuticals stored in smaller bins, more constant size, cleaner environment
- Cat litter (food grade items) environment less controlled, less clean
- Footprint of factory
- Fortunately, within Nestle, they’re in a rural environment, so you can spread out, don’t have to go so high
- In urban environment, you might have to go more vertical
- Tall racks or multi-level warehouse
- Will want to keep bins and racking as constant as you can
- Dictates the type of equipment to reach the high places
- Warehouse with different ceiling heights would require forklift of different extensions (wouldn’t need it in lower portions)
- Walmarts of the world store irregular items (i.e. lawn mowers in one aisle, fingernail clippers in another)
- In well-run warehouses, there’s nothing in the aisles (no litter, refuse). Everything is in a bin. Could create inventory inaccuracies if you have items stacked in the rows, and is a safety hazard. 
- RFIDs
- Tradeoff between having too long a range of detection & too short
- Potential problem - drone trying to read RFID of a package - have a lot of packages within a few meters (warehouses are very dense), and it’s possible for a drone to read multiple RFID tags at once
- Could be okay if you are looking for a particular item of a particular class, but if you’re looking for specific item in specific bin that might be more ambiguous
- Stronger vs. weaker signal
- Some resistance to installing RFID transponders in packages that are low-value. 
- Can be quite expensive. 10 or 20 cents a piece. 
- Maybe for our first run we could do RFIDs and then once we have that working we could shift to barcodes - so we develop incrementally
- Warehouses often use bins to hold their stuff so the barcodes might not be visible - agreed

**Walmart**
- 99 percent accuracy in distribution warehouses
- 70 percent accuracy in retail warehouses

:



📦 Industrial Bakery Simulation



Final Project – Algorithms and Data Structures (2023–2024, Politecnico di Milano)

Grade: 30/30 – Maximum score achieved



📖 Overview



This project implements a discrete-time simulation of an industrial bakery order management system.

The goal is to efficiently manage recipes, ingredient inventory, customer orders, and periodic deliveries, following a precise specification provided for the final exam of Algorithms and Data Structures.



The simulation advances one time unit after each processed command and terminates at the end of the input stream.



🧁 System Model



The bakery system manages:



Ingredients, identified by name and stored in a warehouse



Recipes, each composed of multiple ingredients with required quantities



Ingredient batches, each defined by quantity and expiration time



Customer orders, which may be immediately processed or queued if ingredients are insufficient



A delivery truck, arriving periodically with limited capacity



Ingredient consumption always prioritizes batches with the earliest expiration date.



⏱️ Order Handling



Orders are accepted if the recipe exists, otherwise rejected



If ingredients are insufficient, orders are placed in a pending queue



Pending orders are re-evaluated after each restock, in FIFO order



Prepared orders are stored until picked up by the courier



At delivery time, orders are selected in chronological order, respecting truck capacity



Selected orders are loaded by descending weight, with ties broken by arrival time



🧠 Data Structures Used



The implementation emphasizes efficient data access and update operations:



Hash Tables



Recipes lookup (hash\_table)



Ingredient warehouse (hashmag)



Min-Heaps



Used per ingredient to manage batches ordered by expiration date



Linked Lists



Recipe ingredient lists



Pending orders queue



Ready orders list



Dynamic Arrays



Heap resizing for ingredient batches



Custom hash functions (FNV-based) are used to ensure uniform distribution.



⚙️ Commands Supported



aggiungi\_ricetta – Add a new recipe



rimuovi\_ricetta – Remove a recipe if no pending or ready orders exist



rifornimento – Restock the warehouse with ingredient batches



ordine – Place a customer order



The program prints status messages and delivery outputs exactly as specified.



🛠️ Implementation Details



Language: C



Memory managed manually (malloc, realloc, free)



Designed for efficiency, correctness, and strict adherence to the specification



Handles large input sizes and arbitrary numbers of orders, recipes, and ingredients



✅ Output



At each courier arrival, the program prints:



Loaded orders (time, recipe name, quantity), in loading order



Or camioncino vuoto if no orders are shipped



🎯 Purpose



This project demonstrates the practical application of:



Hashing



Priority queues (heaps)



Linked data structures



Simulation logic



Careful time and state management



All requirements of the official specification are fully implemented.


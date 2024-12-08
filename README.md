```
Στοιχεία φοιτητών που υλοποίησαν την εργασία:
-> Αντωνόπουλος Αναστάσιος ΑΜ:1115201900011
-> Γαγάνης Χαράλαμπος ΑΜ:1115201900030

Οργάνωση του κώδικα:
Όσον αφορά την οργάνωση του Project προσπαθήσαμε να προσεγγίσουμε όσο πιο πολύ μπορούμε την οργάνωση που υπήρχε στις διαφάνειες του φροντιστηρίου.
Υπάρχει δηλαδή η δομή:

root/ 
├── includes/
│ └── utils/ 
│    ├── adjacent.cpp
│    ├── adjacent.h
│    ├── ant_colony.cpp
│    ├── ant_colony.h
│    ├── brute_force.cpp
│    ├── brute_force.h
│    ├── centroid.cpp
│    ├── centroid.h
│    ├── circumcenter.cpp
│    ├── circumcenter.h
│    ├── CMakeLists.txt
│    ├── custom_cdt.h 
│    ├── definitions.h 
│    ├── local_search.cpp
│    ├── local_search.h
│    ├── midpoint.cpp
│    ├── midpoint.h
│    ├── obtuse.cpp 
│    ├── obtuse.h
│    ├── projection.cpp
│    ├── projection.h
│    ├── simulated_annealing.cpp
│    └── simulated_annealing.h
├── CMakeLists.txt
├── README.md
└── opt_triangulation.cpp

Κώδικας στα αρχεία:
->Τα header files έχουν όλα declarations/definitions συναρτήσεων με το εκάστοτε όνομα. Για παράδειγμα το obtuse.h περιέχει declarations συναρτήσεων για έλεγχο obtuse γωνιών/τριγώνων.
->Τα cpp files έχουν τα implementations των αντίστοιχων συναρτήσεων.

Άρα συνδυαστικά, η μέθοδος midpoint έχει το declaration της στο midpoint.h και την υλοποίηση στο midpoint.cpp και ούτω καθεξής.

Μόνη εξαίρεση αποτελεί το αρχείο definitions.h που έχει όλα τα typedef που χρησιμοποιούνται καθώς και όλες τις βιβλιοθήκες που χρησιμοποιούνται παντού.

Ο κώδικας των υλοποιήσεων μαζί με την main() βρίσκονται στο opt_triangulation.cpp.
Στο directory includes/utils υπάρχουν όλα τα απαραίτητα declarations/implementations για συναρτήσεις που χρησιμοποιούνται από το opt_triangulation.cpp.
Με χρήση της CMake δημιουργείται το library Utils που χρησιμοποιείται στο τελικό εκτελέσιμο.

Μεταγλώττιση:
Για να δημιουργηθεί το Makefile του Project αρκεί να τρέξουμε την εντολή:

cmake -DCGAL_DIR=/usr/lib/CGAL .

στο root directory του project, δηλαδή στο ίδιο directory με το αρχείο opt_triangulation.cpp. Αυτή η εντολή θα δημιουργήσει το αρχείο Makefile το οποίο με την εντολή:

make

θα δημιουργήσει με τη σειρά του το εκτελέσιμο αρχείο. Το εκτελέσιμο αρχείο τρέχει με το εξής format:

./opt_triangulation -i <path_to_input.json> -o <path_to_output.json>

Περιγραφή της υλοποίησης:
Αρχικά η υλοποίηση ξεκινάει με Constrained Delaunay Triangulation (CDT).
Έχουμε υλοποιήσει μια συνάρτηση brute_force_steiner_insertion() η οποία ελέγχει ποια θα είναι η βέλτιστη λύση ελέγχοντας ποια από τις υλοποιήσεις εισαγωγής steiner point θα έχει τα λιγότερα insertions.
Αφού ελέγξει όλες τις υλοποιήσεις και βρει τη βέλτιστη, τότε εφαρμόζει τη συνάρτηση apply_best_sequence() η οποία προσθέτει τα steiner points χρησιμοποιώντας τη βέλτιστη υλοποίηση.
Για την εισαγωγή των steiner points έχουν υλοποιηθεί οι εξής μέθοδοι:

-> Εισαγωγή steiner point στο μέσο της μεγαλύτερης ακμής ενός αμβλυγωνίου τριγώνου.
-> Εισαγωγή steiner point στο περίκεντρο (circumcenter) ενός αμβλυγωνίου τριγώνου. Αν το περίκεντρο είναι εκτός του region boundary, το steiner point γίνεται το βαρύκεντρο(centroid).
-> Εισαγωγή steiner point στο σημείο τομής της προβολής της αμβλείας γωνίας με την απέναντι πλευρά.
-> Εισαγωγή steiner point στο κέντρο κυρτού πολυγώνου από γειτωνικά αμβλυγώνια τρίγωνα.

Σε κάθε εκτέλεση του εκτελέσιμου αρχείου θα δημιουργείται ένα αρχείο output.json που θα περιέχει:

1. content_type: Μια συμβολοσειρά "CG_SHOP_2025_Solution".
2. instance_uid: Ένας μοναδικός αναγνωριστικός κωδικός της περίπτωσης (string).
3. steiner_points_x: Μια λίστα με τις x-συντεταγμένες των σημείων Steiner. Κάθε συντεταγμένη μπορεί να είναι ένας ακέραιος ή ένα κλάσμα σε μορφή συμβολοσειράς.
4. steiner_points_y: Μια λίστα με τις y-συντεταγμένες των σημείων Steiner, διαμορφωμένες όπως και οι steiner_points_x.
5. edges: Μια λίστα από ακμές, όπου κάθε ακμή αναπαρίσταται ως ένα ζεύγος ακεραίων που αντιστοιχούν σε δείκτες σημείων.
6. obtuse_count: Ο αριθμός των αμβλυγώνιων τριγώνων.
7. method: Η μέθοδος που χρησιμοποιήθηκε.
8. parameters: Οι παράμετροι που επιλέχθηκαν.

Για τα edges χρησιμοποιήθηκε map το οποίο αντιστοιχεί τα vertices(κορυφές) με ένα δείκτη σημείου.

Στον φάκελο instances συμπεριλαμβάνονται κάποια από τα test instances που δόθηκαν και ο κώδικας συγκλίνει/μειώνει σημαντικά τις αμβλείες.

Αποτελέσματα:

	instance			Local Search		Simulated Annealing		Ant colony
<obtuse, steiner>		<obtuse, steiner>	<obtuse, steiner>		<obtuse, steiner>

instance_2.json			
	<2, 0>				<0 , 3>					<0 , 3>					<1, 1>

instance_3.json
	<4, 0>				<0, 3>					<0, 2>					<0, 4>

instance_4.json
	<10, 0>				<5, 30>					<3, 11>					<10, 0>

instance_5.json
	<10, 0>				<0, 13>					<0, 17>					<10, 0>

instance_test_1.json
	<5, 0>				<0, 10>					<0, 5>					<0, 5>

instance_test_2.json
	<5, 0>				<0, 9>					<0, 4>					<0, 5>

instance_test_3.json
	<5, 0>				<0, 9>					<0, 5>					<0, 5>

instance_test_4.json
	<2, 0>				<0, 1>					<0, 1>					<0, 2>

instance_test_5.json
	<3, 0>				<0, 3>					<0, 3>					<0, 3>

Τα παραπάνω τρέξανε με παραμέτρους:
"parameters": {
        "alpha": 4.0,
        "beta": 0.8,
        "xi": 1,
        "psi": 5,
        "L": 50,
		"lambda": 0.6,
		"kappa": 20
	}
Ωστόσο στο instance_4.json η local_search, αξίζει να σημειωθεί πως για αριθμό επαναλήψεων μεγαλύτερο του 35 αργεί.
Αυτό οφείλεται στο γεγονός ότι σε κάθε επάναληψη απομονώνει τα obtuse faces.
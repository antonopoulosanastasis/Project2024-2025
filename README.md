```
Στοιχεία φοιτητών που υλοποίησαν την εργασία:
-> Αντωνόπουλος Αναστάσιος ΑΜ:1115201900011
-> Γαγάνης Χαράλαμπος ΑΜ:1115201900030

Μεταγλώττιση:
Όσον αφορά την οργάνωση του Project προσπαθήσαμε να προσεγγίσουμε όσο πιο πολύ μπορούμε την οργάνωση που υπήρχε στις διαφάνειες του φροντιστηρίου.
Υπάρχει δηλαδή η δομή:

root/ 
├── includes/
│ └── utils/ 
│    ├── CMakeLists.txt 
│    ├── custom_cdt.h 
│    ├── definitions.h 
│    ├── midpoint.cpp
│    ├── midpoint.h
│    ├── projection.cpp
│    ├── projection.h
│    ├── circumcenter.cpp
│    ├── circumcenter.h
│    ├── brute_force.cpp
│    ├── brute_force.h
│    ├── obtuse.cpp 
│    └── obtuse.h
├── CMakeLists.txt
├── README.md
└── test.cpp

Κώδικας στα αρχεία:
->Τα header files έχουν όλα declarations/definitions συναρτήσεων με το εκάστοτε όνομα. Για παράδειγμα το obtuse.h περιέχει declarations συναρτήσεων για έλεγχο obtuse γωνιών/τριγώνων.
->Τα cpp files έχουν τα implementations των αντίστοιχων συναρτήσεων.

Άρα συνδυαστικά, η μέθοδος midpoint έχει το declaration της στο midpoint.h και την υλοποίηση στο midpoint.cpp και ούτω καθεξής.

Μόνη εξαίρεση αποτελεί το αρχείο definitions.h που έχει όλα τα typedef που χρησιμοποιούνται καθώς και όλες τις βιβλιοθήκες που χρησιμοποιούνται παντού.

Ο κώδικας των υλοποιήσεων μαζί με την main() βρίσκονται στο test.cpp. Στο directory includes/utils υπάρχουν όλα τα απαραίτητα declarations/implementations για συναρτήσεις που χρησιμοποιούνται από το test.cpp. Με χρήση της CMake δημιουργείται το library Utils που χρησιμοποιείται στο τελικό εκτελέσιμο.

Για να δημιουργηθεί το Makefile του Project αρκεί να τρέξουμε την εντολή:

cmake -DCGAL_DIR=/usr/lib/CGAL .

στο root directory του project, δηλαδή στο ίδιο directory με το αρχείο test.cpp. Αυτή η εντολή θα δημιουργήσει το αρχείο Makefile το οποίο με την εντολή:

make

θα δημιουργήσει με τη σειρά του το εκτελέσιμο αρχείο. Το εκτελέσιμο αρχείο τρέχει με το εξής format:

./test input.json <steiner_count>

όπου το steiner_count είναι ο μέγιστος αριθμός των steiner σημείων που μπορούν να προστεθούν στην τριγωνοποίηση μέσω της brute_force(). Επειδή η πολυπλοκότητα της brute_force() είναι εκθετική, πρέπει το steiner_count να έχει μικρή τιμή (max 11) αλλά για αυτά θα βρει τη βέλτιστη λύση, αν υπάρχει. 

Περιγραφή της υλοποίησης:
Αρχικά η υλοποίηση ξεκινάει με Constrained Delaunay Triangulation (CDT). Έχουμε υλοποιήσει μια συνάρτηση brute_force_steiner_insertion() η οποία ελέγχει ποια θα είναι η βέλτιστη λύση ελέγχοντας ποια από τις υλοποιήσεις εισαγωγής steiner point θα έχει τα λιγότερα insertions. Αφού ελέγξει όλες τις υλοποιήσεις και βρει τη βέλτιστη, τότε εφαρμόζει τη συνάρτηση apply_best_sequence() η οποία προσθέτει τα steiner points χρησιμοποιώντας τη βέλτιστη υλοποίηση. Για την εισαγωγή των steiner points έχουν υλοποιηθεί οι εξής μέθοδοι:

-> Εισαγωγή steiner point στο μέσο της μεγαλύτερης ακμής ενός αμβλυγωνίου τριγώνου.
-> Εισαγωγή steiner point στο περίκεντρο (circumcenter) ενός αμβλυγωνίου τριγώνου. Αν το περίκεντρο είναι εκτός του region boundary, το steiner point γίνεται το βαρύκεντρο(centroid).
-> Εισαγωγή steiner point στο σημείο τομής της προβολής της αμβλείας γωνίας με την απέναντι πλευρά.

Σε κάθε εκτέλεση του εκτελέσιμου αρχείου θα δημιουργείται ένα αρχείο output.json που θα περιέχει:

1. content_type: Μια συμβολοσειρά "CG_SHOP_2025_Solution".
2. instance_uid: Ένας μοναδικός αναγνωριστικός κωδικός της περίπτωσης (string).
3. steiner_points_x: Μια λίστα με τις x-συντεταγμένες των σημείων Steiner. Κάθε συντεταγμένη μπορεί να είναι ένας ακέραιος ή ένα κλάσμα σε μορφή συμβολοσειράς.
4. steiner_points_y: Μια λίστα με τις y-συντεταγμένες των σημείων Steiner, διαμορφωμένες όπως και οι steiner_points_x.
5. edges: Μια λίστα από ακμές, όπου κάθε ακμή αναπαρίσταται ως ένα ζεύγος ακεραίων που αντιστοιχούν σε δείκτες σημείων.

Για τα edges χρησιμοποιήθηκε map το οποίο αντιστοιχεί τα vertices(κορυφές) με ένα δείκτη σημείου.

Στον φάκελο instances συμπεριλαμβάνονται κάποια από τα test instances που δόθηκαν και ο κώδικας συγκλίνει/μειώνει σημαντικά τις αμβλείες.

Εντολή: ./test instances/<instance.json> <number of steiner points>

instances που συγκλίνει/μειώνει:

instance_2.json συγκλίνει (6 steiner). 

instance_3.json συγκλίνει. (2 steiner).

instance_4.json μειώνει από 10 αμβλείες σε 1 (9 steiner). 

instance_5.json μειώνει από 10 αμβλείες σε 4 (9 steiner). 

instance_6.json μειώνει από 8 αμβλείες σε 3 (6 steiner). 

instance_7.json συγκλίνει (4 steiner). 

instance_test_4.json συγκλίνει (1 steiner).

instance_test_5.json συγκλίνει (3 steiner).

instance_test_6.json συγκλίνει (6 steiner).

instance_test_7.json συγκλίνει (8 steiner)

instance_test_8.json μειώνει από 5 αμβλείες σε 2 (5 steiner).

instance_test_9.json μειώνει από 7 αμβλείες σε 1 (8 steiner).

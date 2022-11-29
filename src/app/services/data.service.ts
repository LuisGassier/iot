import { Injectable } from '@angular/core';
import { AngularFirestore, AngularFirestoreCollection } from '@angular/fire/compat/firestore';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class DataService {

  constructor( 
    private firestore: AngularFirestore
  ) { }

  //using firestore to get data from firebase 7.5.0

  // get time and value from firebase
  getData(): Observable<any> {
    return this.firestore.collection('data').snapshotChanges();
  }
    
}

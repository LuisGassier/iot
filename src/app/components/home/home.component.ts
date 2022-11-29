import { Component, OnInit } from '@angular/core';
import { Subscription } from 'rxjs';
import { DataService } from 'src/app/services/data.service';

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.css']
})
export class HomeComponent implements OnInit {
  // variable que almacena la data
  loading = false;
  data: any;
  time: any;
  value: number = 0;
  suscriptionData: Subscription = new Subscription();

  constructor(
    private dataService: DataService,
  ) {

  }

  ngOnInit(): void {
    this.loading = true;
    this.getData();
  }

  // funcion que obtiene la data
  getData () {
    this.suscriptionData = this.dataService.getData().subscribe(data => {
      this.data = data.map((e: { payload: { doc: { id: any; data: () => { (): any; new(): any;[x: string]: any; }; }; }; }) => {
        return {
          id: e.payload.doc.id,
          time: e.payload.doc.data()['time'],
          value: e.payload.doc.data()['value']
        }
      })
      // timestamp to date

      this.time = this.data[0].time;
      this.value = this.data[0].value;
      console.log(this.data);
      console.log(this.time);
      console.log(this.value);
      this.loading = false;
    });
    
  }


}

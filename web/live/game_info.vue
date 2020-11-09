<template>
  <div class="game_info">
    <div>
      {{ queue_type.name }}<span class="map">({{ queue_type.map }})</span>
    </div>
    <div class="bans">
      <div class="blue_bans">
        <div v-for="b in blue_bans" :key="b" class="ban">
          <img v-if="b" :src="b.image" />
        </div>
      </div>

      <div class="red_bans">
        <div v-for="b in red_bans" :key="b" class="ban">
          <img v-if="b" :src="b.image" />
        </div>
      </div>
    </div>
  </div>
</template>

<style lang="scss">
  .game_info {
    display: flex;
    align-items: center;
    flex-wrap: wrap;
    justify-content: space-between;

    .map {
      margin-left: 1em;

      color: #555;
    }

    .bans {
      display: flex;

      .blue_bans,
      .red_bans {
        margin: 0px 14px;

        display: flex;

        .ban {
          width: 45px;

          border-bottom: 2px solid;
          border-radius: 6px;
          overflow: hidden;

          img {
            display: block;
            width: 100%;
          }
        }
      }

      .blue_bans .ban {
        background-color: #002;
        border-bottom-color: #44f;
      }

      .red_bans .ban {
        background-color: #200;
        border-bottom-color: #f44;
      }
    }
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";
  import { Prop } from "vue-property-decorator";

  import Champions from "Riot/champions";
  import Queues from "Riot/queues";
  import Teams from "Riot/teams";

  @Component
  export default class extends Vue {
    @Prop() readonly game: any;

    get queue_type() {
      return Queues[this.game.queue_type];
    }

    get blue_bans() {
      const r = this.game.bans.filter(e => e.team === Teams.blue);
      return r.map(e => (e.champion ? Champions[e.champion] : null));
    }

    get red_bans() {
      const r = this.game.bans.filter(e => e.team === Teams.red);
      return r.map(e => (e.champion ? Champions[e.champion] : null));
    }
  }
</script>

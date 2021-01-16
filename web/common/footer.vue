<template>
  <div class="footer">
    <div class="spacing"></div>
    <div class="bottom">
      <a :href="'https://github.com/kiwixz/algue/tree/' + version">algue version <span class="version">{{ version.substr(0, 8) }}</span></a>
      for League of Legends <span class="version">{{ riot_version }}</span>
    </div>
  </div>
</template>

<style lang="scss">
  .footer {
    color: #183018;
    font-size: 0.75em;

    .spacing, .bottom {
      padding: 10px;
    }

    .spacing {
      height: calc(1em + 80px);
    }

    .bottom {
      bottom: 0;
      position: fixed;
      right: 0;

      text-shadow: #000 -1px -1px, #000 1px -1px, #000 -1px 1px, #000 1px 1px;

      .version {
        font-weight: 500;
      }
    }
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";

  import Info from "Generated/info";
  import RiotInfo from "Riot/info";

  @Component
  export default class extends Vue {
    get version() {
      return Info.version;
    }

    get riot_version() {
      return RiotInfo.version;
    }
  }
</script>
